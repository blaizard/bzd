"""clang-tidy aspect rule."""

load("@bazel_tools//tools/build_defs/cc:action_names.bzl", "ACTION_NAMES")
load("@bazel_tools//tools/cpp:toolchain_utils.bzl", "find_cpp_toolchain")

CompileCommandsInfo = provider(
    "Compile command information.",
    fields = {
        "args": "Command line arguments for this compile command.",
        "compiler": "The path of the compiler being used.",
        "files": "Depset of files associated with this compile command.",
        "srcs": "Srcs files associated with this compile command.",
    },
)

def _get_sources(ctx):
    """Get all the sources associated with this target."""

    srcs = []
    if hasattr(ctx.rule.attr, "srcs"):
        for src in ctx.rule.attr.srcs:
            srcs += [f for f in src.files.to_list() if f.basename.endswith((".cc", ".c", ".cpp"))]
    if hasattr(ctx.rule.attr, "hdrs"):
        for hrd in ctx.rule.attr.hdrs:
            srcs += [f for f in hrd.files.to_list() if f.basename.endswith((".hh", ".h", ".hpp"))]
    return srcs

def _get_flags(ctx):
    """Get the current flags from the toolchain."""

    cc_toolchain = find_cpp_toolchain(ctx)
    feature_configuration = cc_common.configure_features(
        ctx = ctx,
        cc_toolchain = cc_toolchain,
    )
    compile_variables = cc_common.create_compile_variables(
        feature_configuration = feature_configuration,
        cc_toolchain = cc_toolchain,
        user_compile_flags = ctx.fragments.cpp.cxxopts + ctx.fragments.cpp.copts,
    )
    flags = cc_common.get_memory_inefficient_command_line(
        feature_configuration = feature_configuration,
        action_name = ACTION_NAMES.cpp_compile,
        variables = compile_variables,
    )
    return flags

def _get_compiler(ctx):
    """Get the interpreter and the dependency files."""

    cc_toolchain = find_cpp_toolchain(ctx)
    return cc_toolchain.compiler_executable, cc_toolchain.all_files

def make_compile_commands(target, ctx):
    """Generate a CompileCommandsInfo provider for all source C/C++ files.

    Args:
        target: The target being evaluated.
        ctx: The context of the action.

    Returns:
        If the target is compatible, a CompileCommandsInfo provider, otherwise None.
    """

    # Only deal with C/C++ targets
    if not CcInfo in target:
        return None

    # Sources
    srcs = _get_sources(ctx)
    compiler, compiler_deps = _get_compiler(ctx)
    files = depset(direct = srcs, transitive = [compiler_deps, target[CcInfo].compilation_context.headers])
    args = []

    if not srcs:
        return None

    # Compiler flags
    args += _get_flags(ctx)

    # Rule flags
    args += ctx.rule.attr.copts if hasattr(ctx.rule.attr, "copts") else []

    # Includes
    args += ["-F" + s for s in target[CcInfo].compilation_context.framework_includes.to_list()]
    args += ["-I" + s for s in target[CcInfo].compilation_context.includes.to_list()]
    args += [segment for s in target[CcInfo].compilation_context.quote_includes.to_list() for segment in ["-iquote", s]]
    args += [segment for s in target[CcInfo].compilation_context.system_includes.to_list() for segment in ["-isystem", s]]

    # Defines
    args += ["-D" + s for s in target[CcInfo].compilation_context.defines.to_list()]
    args += ["-D" + s for s in target[CcInfo].compilation_context.local_defines.to_list()]

    return CompileCommandsInfo(compiler = compiler, srcs = srcs, args = args, files = files)

def _compile_commands_aspect_impl(target, ctx):
    """Generate a compile_command.json file for all C/C++ targets."""

    maybe_compile_commands = make_compile_commands(target, ctx)
    if not maybe_compile_commands:
        return []

    compile_command = []
    for src in maybe_compile_commands.srcs:
        compile_command.append({
            "command": maybe_compile_commands.compiler + " " + " ".join(maybe_compile_commands.args) + " -c " + src.path,
            "directory": ".",
            "file": src.path,
        })

    output = ctx.actions.declare_file("{}.compile_commands.json".format(ctx.label.name))
    ctx.actions.run(
        inputs = maybe_compile_commands.files,
        outputs = [output],
        executable = ctx.attr._client.files_to_run,
        arguments = [output.path] + [f.path for f in maybe_compile_commands.srcs] + ["--", maybe_compile_commands.compiler] + maybe_compile_commands.args,
        mnemonic = "CompileCommands",
        progress_message = "Generating compile commands for {}".format(ctx.label),
    )

    return [
        maybe_compile_commands,
        OutputGroupInfo(compile_commands = depset([output])),
    ]

compile_commands_aspect = aspect(
    implementation = _compile_commands_aspect_impl,
    fragments = ["cpp"],
    attr_aspects = ["deps"],
    attrs = {
        "_client": attr.label(
            doc = "Compile command binary to be used.",
            executable = True,
            cfg = "exec",
            default = Label("//aspects/compile_commands:client"),
        ),
    },
    toolchains = ["@bazel_tools//tools/cpp:toolchain_type"],
)
