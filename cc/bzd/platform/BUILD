load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "types",
    hdrs = [
        "types.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "platform",
    hdrs = [
        "system.h",
    ],
    visibility = ["//visibility:public"],
    deps = select({
        "//tools/bazel_build/config:x86_64": ["//cc/bzd/platform/x86_64_system_v"],
    }),
)

exports_files(
    [
        "system.h",
    ],
    visibility = [
        "//cc/bzd/platform:__subpackages__",
    ],
)
