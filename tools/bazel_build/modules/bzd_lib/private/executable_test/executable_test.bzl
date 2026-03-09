"""Rule to test an executable target."""

load("//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")

def _bzd_executable_test_impl(ctx):
    arguments_executor = []
    if ctx.attr.expected_returncode:
        arguments_executor += ["--expected-returncode", ctx.attr.expected_returncode]
    if ctx.attr.unexpected_returncode:
        arguments_executor += ["--unexpected-returncode", ctx.attr.unexpected_returncode]
    if ctx.attr.expected_output:
        arguments_executor += ["--expected-output", "'{}'".format(ctx.attr.expected_output)]
    if ctx.attr.min_duration != -1:
        arguments_executor += ["--min-duration", str(ctx.attr.min_duration)]
    if ctx.attr.max_duration != -1:
        arguments_executor += ["--max-duration", str(ctx.attr.max_duration)]

    return [sh_binary_wrapper_impl(
        ctx = ctx,
        locations = {
            ctx.attr.executable: "executable",
            ctx.attr._executor: "executor",
        },
        output = ctx.outputs.executable,
        command = "{{executor}} {arguments_executor} {{executable}} -- \"$@\"".format(
            arguments_executor = " ".join(arguments_executor),
        ),
    )]

bzd_executable_test = rule(
    doc = "Test the given executable target.",
    implementation = _bzd_executable_test_impl,
    attrs = {
        "executable": attr.label(
            executable = True,
            cfg = "target",
            mandatory = True,
        ),
        "expected_output": attr.string(
            doc = "An expected string that is contained within the output of the executable.",
        ),
        "expected_returncode": attr.string(
            doc = "The return code expected by the executable.",
        ),
        "max_duration": attr.int(
            doc = "An expected minimal duration in seconds of the executable execution.",
            default = -1,
        ),
        "min_duration": attr.int(
            doc = "An expected minimal duration in seconds of the executable execution.",
            default = -1,
        ),
        "unexpected_returncode": attr.string(
            doc = "An unexpected return code by the executable.",
        ),
        "_executor": attr.label(
            executable = True,
            cfg = "exec",
            default = Label("//private/executable_test:executable_test"),
        ),
    },
    test = True,
)
