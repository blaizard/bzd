load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "assert",
    hdrs = [
        "assert.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":log",
        "//cc/bzd/container:result",
        "//cc/bzd/core:assert",
        "//cc/bzd/core:system",
    ],
)

cc_library(
    name = "log",
    hdrs = [
        "log.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:string",
        "//cc/bzd/container:string_view",
        "//cc/bzd/core:channel",
        "//cc/bzd/core:system",
        "//cc/bzd/utility:forward",
        "//cc/bzd/utility/format",
    ],
)
