load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "system",
    srcs = [
        "stack.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [":headers"] + select({
        "//tools/bazel_build/config:x86_64": [
            "//cc/bzd/platform/bootstrap/posix",
            "//cc/bzd/platform/clock/std",
            "//cc/bzd/platform/out/std",
            "//cc/bzd/platform/panic",
            "//cc/bzd/platform/stack/x86_64_system_v",
        ],
        "@esp32_xtensa_lx6_gcc//:target": [
            "//cc/bzd/platform/clock/esp32_xtensa_lx6",
            "//cc/bzd/platform/out/std",
            "//cc/bzd/platform/panic",
            "//cc/bzd/platform/stack/stub",
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
        "clock.h",
        "out.h",
        "panic.h",
        "stack.h",
    ],
    visibility = ["//cc/bzd/platform:__subpackages__"],
    deps = [
        ":types",
        "//cc/bzd/algorithm:fill",
        "//cc/bzd/core:channel",
        "//cc/bzd/core:units",
        "//cc/bzd/utility:align_down",
        "//cc/bzd/utility:align_up",
    ],
)
