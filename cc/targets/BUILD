alias(
    name = "auto",
    actual = select({
        "@platforms//os:linux": "//cc/targets/linux:auto",
        "@//tools/bazel_build/platforms/isa:xtensa_lx6": "//cc/targets/esp32_xtensa_lx6:auto",
    }),
    visibility = ["//visibility:public"],
)
