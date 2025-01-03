"""clang-tidy aspect rule."""

load("//aspects/compile_commands:defs.bzl", "ATTR_ASPECTS", "get_ouput_group_files_from_attrs", "make_compile_commands")

def _clang_tidy_aspect_impl(target, ctx):
    """Run clang-tidy on the target."""

    compile_commands_list = make_compile_commands(target, ctx)
    if not compile_commands_list:
        return [OutputGroupInfo(clang_tidy = get_ouput_group_files_from_attrs(ctx, "clang_tidy"))]

    outputs = []
    for compile_commands in compile_commands_list:
        for src in compile_commands.srcs:
            # The label name should also be used to avoid a conflict with 2 cc_library rules with the shared file for example.
            output = ctx.actions.declare_file("{}.{}.clang_tidy".format(src.short_path.replace("/", "."), ctx.label.name))
            ctx.actions.run(
                inputs = compile_commands.files,
                outputs = [output],
                executable = ctx.attr._clang_tidy.files_to_run,
                arguments = [output.path, src.short_path, "--"] + compile_commands.args,
                mnemonic = "ClangTidy",
                progress_message = "Clang-tidying {}".format(src.short_path),
            )
            outputs.append(output)

    return [
        OutputGroupInfo(clang_tidy = get_ouput_group_files_from_attrs(ctx, "clang_tidy", outputs)),
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
    attr_aspects = ATTR_ASPECTS,
    toolchains = ["@bazel_tools//tools/cpp:toolchain_type"],
)
