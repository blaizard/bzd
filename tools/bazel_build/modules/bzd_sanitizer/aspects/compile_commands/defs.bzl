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

ATTR_ASPECTS = ["deps", "system"]  # To support bdl_* rules

def get_ouput_group_files_from_attrs(ctx, name, additional_files = None):
    """Get all output_group from attributes.

    Args:
        ctx: the context of the action.
        name: The output group info name.
        additional_files: additional files to be added.

    Returns:
        A depset containing all the files.
    """

    transitive = []
    for attr in ATTR_ASPECTS:
        if hasattr(ctx.rule.attr, attr):
            value = getattr(ctx.rule.attr, attr)
            deps = value if type(value) == "list" else [value]
            for dep in deps:
                if type(dep) != "Target":
                    continue
                if OutputGroupInfo in dep and hasattr(dep[OutputGroupInfo], name):
                    transitive.append(getattr(dep[OutputGroupInfo], name))
    return depset(additional_files or [], transitive = transitive)

def _get_sources(ctx):
    """Get all the sources associated with this target."""

    srcs_c = []
    srcs_cc = []
    if hasattr(ctx.rule.attr, "srcs"):
        for src in ctx.rule.attr.srcs:
            srcs_c += [f for f in src.files.to_list() if f.extension.lower() in ("c",)]
            srcs_cc += [f for f in src.files.to_list() if f.extension.lower() in ("cc", "cpp", "c++", "cxx")]
    if hasattr(ctx.rule.attr, "hdrs"):
        for hrd in ctx.rule.attr.hdrs:
            srcs_c += [f for f in hrd.files.to_list() if f.extension.lower() in ("h",)]
            srcs_cc += [f for f in hrd.files.to_list() if f.extension.lower() in ("hh", "hpp", "h++", "hxx")]
    return srcs_c, srcs_cc

def _get_flags(ctx, action_name, user_compile_flags, compilation_contexts):
    """Get the current flags from the toolchain."""

    cc_toolchain = find_cpp_toolchain(ctx)
    feature_configuration = cc_common.configure_features(
        ctx = ctx,
        cc_toolchain = cc_toolchain,
    )
    compile_variables = cc_common.create_compile_variables(
        feature_configuration = feature_configuration,
        cc_toolchain = cc_toolchain,
        user_compile_flags = user_compile_flags,
        include_directories = depset(transitive = [compilation_context.includes for compilation_context in compilation_contexts]),
        quote_include_directories = depset(transitive = [compilation_context.quote_includes for compilation_context in compilation_contexts]),
        system_include_directories = depset(transitive = [compilation_context.system_includes for compilation_context in compilation_contexts]),
        framework_include_directories = depset(transitive = [compilation_context.framework_includes for compilation_context in compilation_contexts]),
        preprocessor_defines = depset(transitive = [compilation_context.defines for compilation_context in compilation_contexts] + [compilation_context.local_defines for compilation_context in compilation_contexts]),
    )
    flags = cc_common.get_memory_inefficient_command_line(
        feature_configuration = feature_configuration,
        action_name = action_name,
        variables = compile_variables,
    )

    # Flags to ignore
    ignore = ("-fno-canonical-system-headers", "-mlongcalls", "-fno-tree-switch-conversion")
    return [f for f in flags if not (f in ignore)]

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
    if not (CcInfo in target or "cc_" in ctx.rule.kind):
        return []

    # Sources
    srcs_c, srcs_cc = _get_sources(ctx)
    compiler, compiler_deps = _get_compiler(ctx)
    compilation_contexts = [target[CcInfo].compilation_context] if CcInfo in target else []
    if hasattr(ctx.rule.attr, "deps"):
        compilation_contexts += [dep[CcInfo].compilation_context for dep in ctx.rule.attr.deps if CcInfo in dep]
    files = depset(direct = srcs_c + srcs_cc, transitive = [compiler_deps] + [compilation_context.headers for compilation_context in compilation_contexts])
    user_compile_flags = ctx.rule.attr.copts if hasattr(ctx.rule.attr, "copts") else []

    providers = []
    if srcs_c:
        args = _get_flags(
            ctx = ctx,
            action_name = ACTION_NAMES.c_compile,
            user_compile_flags = user_compile_flags + ctx.fragments.cpp.copts,
            compilation_contexts = compilation_contexts,
        )
        providers.append(CompileCommandsInfo(compiler = compiler, srcs = srcs_c, args = args, files = files))
    if srcs_cc:
        args = _get_flags(
            ctx = ctx,
            action_name = ACTION_NAMES.cpp_compile,
            user_compile_flags = user_compile_flags + ctx.fragments.cpp.cxxopts + ctx.fragments.cpp.copts,
            compilation_contexts = compilation_contexts,
        )
        providers.append(CompileCommandsInfo(compiler = compiler, srcs = srcs_cc, args = args, files = files))

    return providers

def _compile_commands_aspect_impl(target, ctx):
    """Generate a compile_command.json file for all C/C++ targets."""

    compile_commands_list = make_compile_commands(target, ctx)
    if not compile_commands_list:
        return [OutputGroupInfo(compile_commands = get_ouput_group_files_from_attrs(ctx, "compile_commands"))]

    compile_commands_files = []
    for index, compile_commands in enumerate(compile_commands_list):
        output = ctx.actions.declare_file("{}.{}.compile_commands.json".format(ctx.label.name, index))
        ctx.actions.run(
            inputs = compile_commands.files,
            outputs = [output],
            executable = ctx.attr._client.files_to_run,
            arguments = [output.path] + [f.path for f in compile_commands.srcs] + ["--", compile_commands.compiler] + compile_commands.args,
            mnemonic = "CompileCommands",
            progress_message = "Generating compile commands for {}".format(ctx.label),
        )
        compile_commands_files.append(output)

    return [OutputGroupInfo(compile_commands = get_ouput_group_files_from_attrs(ctx, "compile_commands", compile_commands_files))]

compile_commands_aspect = aspect(
    implementation = _compile_commands_aspect_impl,
    fragments = ["cpp"],
    attr_aspects = ATTR_ASPECTS,
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
