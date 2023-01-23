load("@rules_python//python:pip.bzl", "pip_parse")

def bazel_rules_python_register():
    pip_parse(
        name = "python_deps",
        requirements_lock = "//tools/bazel_build/rules/third_party/python:requirements.txt",
    )
