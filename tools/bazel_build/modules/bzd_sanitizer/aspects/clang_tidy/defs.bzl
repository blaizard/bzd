"""clang-tidy aspect rule."""

load("@bazel_tools//tools/build_defs/cc:action_names.bzl", "ACTION_NAMES")
load("@bazel_tools//tools/cpp:toolchain_utils.bzl", "find_cpp_toolchain")

def _get_sources(ctx):
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

def _clang_tidy_aspect_impl(target, ctx):
    """Run Clang-tidy on the target."""

    # Only deal with C/C++ targets
    if not CcInfo in target:
        return []

    args = ctx.actions.args()

    # Compiler flags
    flags = _get_flags(ctx)
    args.add_all(flags)

    # Rule flags
    args.add_all(ctx.rule.attr.copts if hasattr(ctx.rule.attr, "copts") else [])

    # Includes
    args.add_all(target[CcInfo].compilation_context.framework_includes.to_list(), before_each = "-F")
    args.add_all(target[CcInfo].compilation_context.includes.to_list(), before_each = "-I")
    args.add_all(target[CcInfo].compilation_context.quote_includes.to_list(), before_each = "-iquote")
    args.add_all(target[CcInfo].compilation_context.system_includes.to_list(), before_each = "-isystem")

    # Defines
    args.add_all(target[CcInfo].compilation_context.defines.to_list(), before_each = "-D")
    args.add_all(target[CcInfo].compilation_context.local_defines.to_list(), before_each = "-D")

    # Sources
    outputs = []
    srcs = _get_sources(ctx)

    for src in srcs:
        # The label name should also be used to avoid a conflict with 2 cc_library rules with the shared file for example.
        output = ctx.actions.declare_file("{}.{}.clang_tidy".format(src.short_path.replace("/", "."), ctx.label.name))
        inputs = depset(direct = [src], transitive = [target[CcInfo].compilation_context.headers])
        ctx.actions.run(
            inputs = inputs,
            outputs = [output],
            executable = ctx.attr._clang_tidy.files_to_run,
            arguments = [output.path, src.short_path, "--", args],
            mnemonic = "ClangTidy",
            progress_message = "Clang-tidying {}".format(src.short_path),
        )
        outputs.append(output)

    # Get the outputs from the dependencies.
    for dep in ctx.rule.attr.deps:
        outputs += dep[OutputGroupInfo].clang_tidy.to_list()

    return [
        OutputGroupInfo(clang_tidy = depset(direct = outputs)),
    ]

clang_tidy_aspect = aspect(
    implementation = _clang_tidy_aspect_impl,
    fragments = ["cpp"],
    attrs = {
        "_clang_tidy": attr.label(
            doc = "Clang-tidy binary to be used.",
            executable = True,
            cfg = "exec",
            default = Label("//aspects/clang_tidy"),
        ),
    },
    attr_aspects = ["deps"],
    toolchains = ["@bazel_tools//tools/cpp:toolchain_type"],
)
