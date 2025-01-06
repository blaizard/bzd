load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "pthread",
    linkopts = [
        "-lpthread",
        "-pthread",
    ],
    target_compatible_with = [
        "@bzd_platforms//al:posix",
    ],
    visibility = ["//visibility:public"],
)
