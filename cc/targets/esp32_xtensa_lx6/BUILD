load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "composition",
    srcs = [
        "composition.bdl",
    ],
    deps = [
        "//cc/components/generic/clock/null",
        "//cc/components/generic/executor",
        "//cc/components/generic/stream/stub",
        "//cc/components/std/stream/out",
        "//cc/components/xtensa/clock/xthal",
    ],
)

cc_library(
    name = "esp32_xtensa_lx6",
    srcs = select({
        "//tools/bazel_build/config:test": [],
        "//tools/bazel_build/config:binary": ["main.cc"],
    }),
    target_compatible_with = [
        "@esp32_xtensa_lx6_gcc//:compiler",
    ],
    visibility = [
        "//cc/targets:__pkg__",
    ],
    deps = [
        ":composition",
        "//cc/components/generic/panic/trap",
        "//cc/components/std/atomic",
    ],
)

alias(
    name = "atomic",
    actual = "//cc/components/std/atomic",
    visibility = [
        "//cc/targets:__pkg__",
    ],
)
