load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "system",
    srcs = [
        "stack.cc",
    ],
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
        "out.h",
        "panic.h",
        "stack.h",
        "types.h",
    ],
    visibility = ["//cc/bzd/platform:__subpackages__"],
    deps = [
        "//cc/bzd/core:channel",
        "//cc/bzd/algorithm:fill",
        "//cc/bzd/utility:align_down",
        "//cc/bzd/utility:align_up",
    ],
)
