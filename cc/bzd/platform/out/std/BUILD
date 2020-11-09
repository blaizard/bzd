load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "std",
    srcs = [
        "std.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:headers",
    ],
    alwayslink = True,
)
