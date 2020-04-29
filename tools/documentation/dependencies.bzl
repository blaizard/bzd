load("//tools/documentation/bazel:dependencies.bzl", "documentation_bazel_load")
load("//tools/documentation/cc:dependencies.bzl", "documentation_cc_load")

def documentation_load():
    documentation_bazel_load()
    documentation_cc_load()
