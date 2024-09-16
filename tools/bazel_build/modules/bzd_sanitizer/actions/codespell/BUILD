load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper")
load("@rules_python//python:defs.bzl", "py_binary")
load("@rules_python//python/entry_points:py_console_script_binary.bzl", "py_console_script_binary")

py_console_script_binary(
    name = "codespell_binary",
    pkg = "@bzd_sanitizer_pip//codespell",
    script = "codespell",
    visibility = ["//visibility:public"],
)

py_binary(
    name = "cli",
    srcs = [
        "cli.py",
    ],
    main = "cli.py",
    deps = [
        "//python/bzd_sanitizer:worker",
        "@bzd_python//bzd/utils:run",
    ],
)

sh_binary_wrapper(
    name = "codespell",
    command = "{binary} --codespell {codespell} $@",
    locations = {
        ":cli": "binary",
        ":codespell_binary": "codespell",
    },
    visibility = ["//visibility:public"],
)
