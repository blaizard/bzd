"""clang-tidy aspect rule."""

load("//aspects/compile_commands:defs.bzl", "make_compile_commands")

def _clang_tidy_aspect_impl(target, ctx):
    """Run Clang-tidy on the target."""

    maybe_compile_commands = make_compile_commands(target, ctx)
    if not maybe_compile_commands:
        return []

    outputs = []
    for src in maybe_compile_commands.srcs:
        # The label name should also be used to avoid a conflict with 2 cc_library rules with the shared file for example.
        output = ctx.actions.declare_file("{}.{}.clang_tidy".format(src.short_path.replace("/", "."), ctx.label.name))
        ctx.actions.run(
            inputs = maybe_compile_commands.files,
            outputs = [output],
            executable = ctx.attr._clang_tidy.files_to_run,
            arguments = [output.path, src.short_path, "--"] + maybe_compile_commands.args,
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
