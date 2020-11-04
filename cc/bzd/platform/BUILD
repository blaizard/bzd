load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "types",
    hdrs = [
        "types.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "system",
    visibility = ["//visibility:public"],
    deps = [":headers"] + select({
        "//tools/bazel_build/config:x86_64": [
            "//cc/bzd/platform/x86_64_system_v/context",
            "//cc/bzd/platform/panic",
            "//cc/bzd/platform/log",
            "//cc/bzd/platform/posix/bootstrap",
        ],
    }),
)

cc_library(
    name = "headers",
    hdrs = [
        "stack.h",
        "system.h",
        "types.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/core:channel",
    ],
)
