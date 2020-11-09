load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "system",
    visibility = ["//visibility:public"],
    deps = [":headers"] + select({
        "//tools/bazel_build/config:x86_64": [
            "//cc/bzd/platform/stack/x86_64_system_v",
            "//cc/bzd/platform/panic",
            "//cc/bzd/platform/out/std",
            "//cc/bzd/platform/bootstrap/posix",
        ],
        "@esp32_xtensa_lx6_gcc//:target": [
            "//cc/bzd/platform/panic",
            "//cc/bzd/platform/out/std",
        ],
    }),
)

cc_library(
    name = "types",
    hdrs = [
        "types.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "headers",
    hdrs = [
        "stack.h",
        "out.h",
        "panic.h",
        "types.h",
    ],
    visibility = ["//cc/bzd/platform:__subpackages__"],
    deps = [
        "//cc/bzd/core:channel",
    ],
)
