load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "platform",
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform/backend",
    ],
)

cc_library(
    name = "types",
    hdrs = [
        "types.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "panic",
    hdrs = [
        "panic.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":types",
    ],
)

cc_library(
    name = "atomic",
    hdrs = [
        "atomic.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:named_type",
    ],
)

cc_library(
    name = "stream",
    hdrs = [
        "stream.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/core:channel",
    ],
)

cc_library(
    name = "clock",
    hdrs = [
        "clock.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/core:clock"
    ],
)
