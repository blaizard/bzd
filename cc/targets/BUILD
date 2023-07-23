alias(
    name = "auto",
    actual = select({
        "@bzd_platforms//al:linux": "//cc/targets/linux:auto",
        "@bzd_platforms//isa:xtensa_lx6": "//cc/targets/esp32_xtensa_lx6:auto",
    }),
    visibility = ["//visibility:public"],
)
