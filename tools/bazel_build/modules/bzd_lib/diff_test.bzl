"""Rule to test the equality of 2 files."""

load("//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")

def _bzd_diff_test_impl(ctx):
    arguments_diff = []

    return [sh_binary_wrapper_impl(
        ctx = ctx,
        locations = {
            ctx.attr.file1: "file1",
            ctx.attr.file2: "file2",
            ctx.attr._diff: "diff",
        },
        output = ctx.outputs.executable,
        command = "{{diff}} {arguments_diff} {{file1}} {{file2}}".format(
            arguments_diff = " ".join(arguments_diff),
        ),
    )]

bzd_diff_test = rule(
    doc = "Test the equality of 2 files.",
    implementation = _bzd_diff_test_impl,
    attrs = {
        "file1": attr.label(
            allow_single_file = True,
            mandatory = True,
        ),
        "file2": attr.label(
            allow_single_file = True,
            mandatory = True,
        ),
        "_diff": attr.label(
            executable = True,
            cfg = "exec",
            default = Label("//:diff_test"),
        ),
    },
    test = True,
)
