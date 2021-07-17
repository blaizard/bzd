load("@rules_python//python:pip.bzl", "pip_install")

def bazel_rules_python_register():
    pip_install(
        name = "python_deps",
        requirements = "//tools/bazel_build/rules/third_party/python:requirements.txt",
    )
