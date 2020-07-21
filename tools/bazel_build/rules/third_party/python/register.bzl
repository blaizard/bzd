load("@rules_python//python:repositories.bzl", "py_repositories")
load("@rules_python//python:pip.bzl", "pip3_import", "pip_repositories")

def bazel_rules_python_register():
    py_repositories()
    pip_repositories()
    pip3_import(
        name = "python_deps",
        requirements = "//tools/bazel_build/rules/third_party/python:requirements.txt",
    )
