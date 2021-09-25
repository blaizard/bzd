load("//tools/bazel_build/rules:cc.bzl", "cc_library")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "core",
    srcs = [
        "core.bdl",
    ],
    visibility = ["//visibility:public"],
)

bdl_library(
    name = "executor",
    srcs = [
        "executor.bdl",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "platform",
    visibility = ["//visibility:public"],
    deps = [":headers"] + select({
        "//tools/bazel_build/config:x86_64": [
            "//cc/bzd/platform/bootstrap/posix",
            "//cc/bzd/platform/clock/std",
            #"//cc/bzd/platform/panic",
            "//cc/bzd/platform/panic/exit",
            "//cc/bzd/platform/linux:composition",
        ],
        "@esp32_xtensa_lx6_gcc//:target": [
            "//cc/bzd/platform/clock/esp32_xtensa_lx6",
            "//cc/bzd/platform/out/std",
            "//cc/bzd/platform/panic",
        ],
    }),
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
    name = "headers",
    hdrs = [
        "clock.hh",
        "out.hh",
    ],
    visibility = ["//cc/bzd/platform:__subpackages__"],
    deps = [
        ":atomic",
        ":panic",
        ":types",
        "//cc/bzd/algorithm:fill",
        "//cc/bzd/core:channel",
        "//cc/bzd/core:units",
        "//cc/bzd/utility:align_down",
        "//cc/bzd/utility:align_up",
    ],
)
