"""clang-tidy aspect rule."""

load("@bazel_tools//tools/build_defs/cc:action_names.bzl", "ACTION_NAMES")
load("@bazel_tools//tools/cpp:toolchain_utils.bzl", "find_cpp_toolchain")
load("@rules_cc//cc:defs.bzl", "CcInfo")
load("@rules_cc//cc/common:cc_common.bzl", "cc_common")

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

def _get_minimal_parameters(parameters):
    """Cleanup the parameter list and only keep the parameters useful for a minimal build."""

    to_keep = (
        "-std",
        "-I",
        "-iquote",
        "-isystem",
        "-idirafter",
        "-iprefix",
        "-iwithprefix",
        "-iwithprefixbefore",
        "-isysroot",
        "-imultilib",
        "-nostdinc",
        "-nostdinc++",
        "--embed-dir",
        "-L",
        "-B",
        "--sysroot",
        "-D",
        "-U",
        "-W",
    )

    updated_parameters = []
    for param in parameters:
        if param.startswith(to_keep) or not param.startswith("-"):
            updated_parameters.append(param)
    return updated_parameters

def _get_parameters(ctx, action_name, user_compile_flags, compilation_contexts):
    """Get the current parameters from the toolchain."""

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
    parameters = cc_common.get_memory_inefficient_command_line(
        feature_configuration = feature_configuration,
        action_name = action_name,
        variables = compile_variables,
    )

    return _get_minimal_parameters(parameters)

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
        args = _get_parameters(
            ctx = ctx,
            action_name = ACTION_NAMES.c_compile,
            user_compile_flags = user_compile_flags + ctx.fragments.cpp.copts,
            compilation_contexts = compilation_contexts,
        )
        providers.append(CompileCommandsInfo(compiler = compiler, srcs = srcs_c, args = args, files = files))
    if srcs_cc:
        args = _get_parameters(
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

    compile_commands_content = []
    for compile_commands in compile_commands_list:
        for src in compile_commands.srcs:
            compile_commands_content.append({
                "command": "{} {} -c {}".format(compile_commands.compiler, " ".join(compile_commands.args), src.path),
                "directory": ".",
                "file": src.path,
            })

    output = ctx.actions.declare_file("{}.compile_commands.json".format(ctx.label.name))
    ctx.actions.write(output, content = json.encode(compile_commands_content))

    return [OutputGroupInfo(compile_commands = get_ouput_group_files_from_attrs(ctx, "compile_commands", [output]))]

compile_commands_aspect = aspect(
    implementation = _compile_commands_aspect_impl,
    fragments = ["cpp"],
    attr_aspects = ATTR_ASPECTS,
    toolchains = ["@bazel_tools//tools/cpp:toolchain_type"],
)
