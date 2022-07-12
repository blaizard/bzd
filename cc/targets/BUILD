load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "targets",
    visibility = ["//cc/bzd/platform:__pkg__"],
    deps = select({
        "@platforms//os:linux": ["//cc/targets/linux"],
        "@//tools/bazel_build/platforms/isa:xtensa_lx6": ["//cc/targets/esp32_xtensa_lx6"],
    }),
)
