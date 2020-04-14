load("//tools/documentation/bazel:dependencies.bzl", "load_documentation_bazel_dependencies")
load("//tools/documentation/cc:dependencies.bzl", "load_documentation_cc_dependencies")

def load_documentation_dependencies():
    load_documentation_bazel_dependencies()
    load_documentation_cc_dependencies()
