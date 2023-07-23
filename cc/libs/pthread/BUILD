load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "pthread",
    linkopts = [
        "-lpthread",
        "-pthread",
    ],
    target_compatible_with = select({
        "@esp32//:toolchain": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
    visibility = ["//visibility:public"],
)
