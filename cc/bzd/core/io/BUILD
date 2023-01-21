load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "io",
    hdrs = [
        "buffer.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container/threadsafe:ring_buffer",
        "//cc/bzd/core/async",
    ],
)
