load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "ipv4",
    srcs = [
        "ipv4.cc",
    ],
    hdrs = [
        "ipv4.hh",
    ],
    tags = ["manual"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//cc/bzd/container:result",
        "//cc/bzd/core:error",
        "//cc/components/posix:error",
    ],
)
