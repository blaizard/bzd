load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "atomic",
    srcs = [
        "bzd/atomic.hh",
    ],
    includes = [
        ".",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:named_type",
    ],
)
