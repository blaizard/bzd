load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_target", "bdl_target_platform")

cc_library(
    name = "main",
    srcs = ["main.cc"],
    local_defines = select({
        "//toolchains/cc/fragments/esptool:is_sim": ["BZD_EXECUTOR_SIM=1"],
        "//conditions:default": [],
    }),
    target_compatible_with = [
        "@//tools/bazel_build/platforms/isa:xtensa_lx6",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:panic",
        "//cc/components/generic/clock/null",
        "//cc/components/generic/executor",
        "//cc/components/generic/stream/stub",
        "//cc/components/std/stream/out",
        "//cc/components/xtensa/core",
        "//cc/components/xtensa/timer/gptimer",
        "//interfaces:stream",
    ],
)

bdl_target(
    name = "auto",
    composition = [
        "composition.bdl",
    ],
    language = "cc",
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":main",
    ],
)

bdl_target_platform(
    name = "gcc",
    platform = "@esp32_xtensa_lx6_gcc//:platform",
    target = ":auto",
    visibility = [
        "//visibility:public",
    ],
)
