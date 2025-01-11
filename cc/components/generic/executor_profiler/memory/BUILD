load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "memory",
    hdrs = [
        "memory.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:ring_buffer",
        "//cc/bzd/container:variant",
        "//cc/bzd/core:channel",
        "//cc/bzd/core/async",
        "//cc/bzd/core/async:executor_profiler",
    ],
)
