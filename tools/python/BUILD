load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper")
load("@rules_python//python:pip.bzl", "compile_pip_requirements")
load("@rules_python//python/entry_points:py_console_script_binary.bzl", "py_console_script_binary")

exports_files([
    "requirements.txt",
    "pyproject.toml",
])

sh_binary_wrapper(
    name = "yapf",
    command = "{binary} --config {config} $@",
    locations = {
        "@bzd_sanitizer//actions/yapf": "binary",
        "pyproject.toml": "config",
    },
    visibility = ["//visibility:public"],
)

# bazel run //tools/python:requirements.update
compile_pip_requirements(
    name = "requirements",
    timeout = "long",
    generate_hashes = False,
    requirements_in = ":requirements.in",
    requirements_txt = ":requirements.txt",
    tags = ["manual"],
)

py_console_script_binary(
    name = "mypy",
    pkg = "@pip_linux_x86_64//mypy",
    script = "mypy",
    tags = ["manual"],
    visibility = ["//visibility:public"],
)
