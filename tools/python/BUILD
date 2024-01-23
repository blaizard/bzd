load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper")
load("@rules_python//python:pip.bzl", "compile_pip_requirements")

exports_files(["requirements.txt"])

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
    generate_hashes = False,
    requirements_in = ":requirements.in",
    requirements_txt = ":requirements.txt",
)
