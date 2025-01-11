"""Rule to test an executable target."""

load("//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")

def _bzd_executable_test_impl(ctx):
    arguments_executor = []
    if ctx.attr.expected_returncode:
        arguments_executor += ["--expected-returncode", ctx.attr.expected_returncode]
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

def _bzd_executable_test_attrs_factory(cfg):
    return {
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
        "_executor": attr.label(
            executable = True,
            cfg = cfg,
            default = Label("//:executable_test"),
        ),
    }

bzd_executable_test = rule(
    doc = "Test the given executable target.",
    implementation = _bzd_executable_test_impl,
    attrs = _bzd_executable_test_attrs_factory(cfg = "target"),
    test = True,
)

bzd_executable_exec_test = rule(
    doc = "Test the given executable target using the execution platform.",
    implementation = _bzd_executable_test_impl,
    attrs = _bzd_executable_test_attrs_factory(cfg = "exec"),
    test = True,
)
