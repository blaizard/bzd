load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "composition",
    srcs = [
        "composition.bdl",
    ],
    deps = [
        "//cc/components/generic/clock/null",
        "//cc/components/generic/core",
        "//cc/components/generic/executor",
        "//cc/components/generic/stream/stub",
        "//cc/components/std/stream/out",
        "//cc/components/xtensa/clock/xthal",
    ],
)

cc_library(
    name = "esp32_xtensa_lx6",
    srcs = ["main.cc"],
    target_compatible_with = [
        "@//tools/bazel_build/platforms/isa:xtensa_lx6",
    ],
    visibility = [
        "//cc/targets:__pkg__",
    ],
    deps = [
        ":composition",
        "//cc:bzd",
        "//cc/components/generic/panic/trap",
        "//cc/components/std/stream/out",
    ],
)
