load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "terminate",
    srcs = [
        "terminate.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:panic",
        "//cc/bzd/platform:types",
    ],
    alwayslink = True,
)
