load("@bzd_bdl//:defs.bzl", "bdl_target")
load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "main",
    srcs = ["main.cc"],
    local_defines = select({
        "//tools/bazel_build/settings/executor:is_default": ["BZD_EXECUTOR_SIM=1"],
        "//tools/bazel_build/settings/executor:is_sim": ["BZD_EXECUTOR_SIM=1"],
        "//conditions:default": [],
    }),
    target_compatible_with = [
        "@bzd_platforms//isa:xtensa_lx7",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:panic",
        "//cc/components/esp_idf/core",
        "//cc/components/esp_idf/timer/gptimer",
        "//cc/components/generic/clock/null",
        "//cc/components/generic/executor",
        "//cc/components/generic/stream/stub",
        "//cc/components/std/stream/out",
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

bdl_target(
    name = "gcc",
    parent = ":auto",
    platform = "@esp32s3//:platform-esp32s3_idf-xtensa_lx7",
    visibility = [
        "//visibility:public",
    ],
)
