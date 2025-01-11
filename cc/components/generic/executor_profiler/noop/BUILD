load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "noop",
    srcs = [
        "noop.cc",
    ],
    hdrs = [
        "noop.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/core/async:executor_profiler",
        "//cc/bzd/platform:types",
    ],
)
