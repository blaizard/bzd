"""Rule to test an executable target."""

load("//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")
load("//lib:attrs.bzl", "ATTRS_COMMON_BUILD_RULES", "attrs_assert_any_of")

def _host_platform_transition_impl(_settings, attr):
    if attr.run_on_host_platform:
        return {"//command_line_option:platforms": "@bazel_tools//tools:host_platform"}
    return None

_host_platform_transition = transition(
    implementation = _host_platform_transition_impl,
    inputs = [],
    outputs = ["//command_line_option:platforms"],
)

def _bzd_executable_test_impl(ctx):
    return [sh_binary_wrapper_impl(
        ctx = ctx,
        locations = {
            ctx.attr.executable: "executable",
            ctx.attr._executor[0]: "executor",
        },
        output = ctx.outputs.executable,
        command = "{{executor}} {arguments_executor} {{executable}} -- {arguments_executable}".format(
            arguments_executor = " ".join(ctx.attr.arguments_executor),
            arguments_executable = " ".join(ctx.attr.arguments_executable),
        ),
    )]

_bzd_executable_test = rule(
    implementation = _bzd_executable_test_impl,
    attrs = {
        "arguments_executable": attr.string_list(
            doc = "The list of string to be used as argument to the underlying executable.",
        ),
        "arguments_executor": attr.string_list(
            doc = "The list of string to be used as argument to the underlying executor.",
        ),
        "executable": attr.label(
            executable = True,
            cfg = "target",
            mandatory = True,
        ),
        "run_on_host_platform": attr.bool(
            doc = "Whether or not the test executor should run on the host platform.",
        ),
        "_executor": attr.label(
            executable = True,
            cfg = _host_platform_transition,
            default = Label("//:executable_test"),
        ),
    },
    test = True,
)

def bzd_executable_test(name, executable, args = None, expected_returncode = None, expected_output = None, min_duration = None, max_duration = None, run_on_host_platform = False, **kwargs):
    """Test the given executable target.

    Args:
        name: The name of the rule.
        executable: The executable to be tested.
        args: Extra arguments to be passed to the executable.
        expected_returncode: The return code expected by the executable.
        expected_output: An expected string that is contained within the output of the executable.
        min_duration: An expected minimal duration in seconds of the executable execution.
        max_duration: An expected maximal duration in seconds of the executable execution.
        run_on_host_platform: Whether or not the test executor should run on the host platform.
        **kwargs: extra common build rules attributes.
    """

    attrs_assert_any_of(kwargs, ATTRS_COMMON_BUILD_RULES)

    _bzd_executable_test(
        name = name,
        arguments_executor = ([] if expected_returncode == None else ["--expected-returncode", str(expected_returncode)]) +
                             ([] if expected_output == None else ["--expected-output", "'{}'".format(expected_output)]) +
                             ([] if min_duration == None else ["--min-duration", str(min_duration)]) +
                             ([] if max_duration == None else ["--max-duration", str(max_duration)]),
        arguments_executable = args,
        executable = executable,
        run_on_host_platform = run_on_host_platform,
        **kwargs
    )
