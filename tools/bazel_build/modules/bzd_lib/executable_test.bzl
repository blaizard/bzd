"""Rule to test an executable target."""

load("@rules_python//python:defs.bzl", "py_test")
load("//lib:attrs.bzl", "ATTRS_COMMON_BUILD_RULES", "attrs_assert_any_of")

def bzd_executable_test(name, executable, args = None, expected_returncode = None, expected_output = None, **kwargs):
    """Test the given executable target.

    Args:
        name: The name of the rule.
        executable: The executable to be tested.
        args: Extra arguments to be passed to the executable.
        expected_returncode: The return code expected by the executable.
        expected_output: An expected string that is contained within the output of the executable.
        **kwargs: extra common build rules attributes.
    """

    attrs_assert_any_of(kwargs, ATTRS_COMMON_BUILD_RULES)

    py_test(
        name = name,
        main = Label("//:executable_test.py"),
        srcs = [
            Label("//:executable_test.py"),
        ],
        args = ([] if expected_returncode == None else ["--expected-returncode", str(expected_returncode)]) +
               ([] if expected_output == None else ["--expected-output", "'{}'".format(expected_output)]) +
               ["$(rootpath {})".format(executable), "--"] +
               (args or []),
        data = [
            executable,
        ],
        deps = [
            "@bzd_python//bzd/utils:run",
        ],
        **kwargs
    )
