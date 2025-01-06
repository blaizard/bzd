alias(
    name = "auto",
    actual = select({
        "@bzd_platforms//al:linux": "//cc/targets/linux:auto",
        "@bzd_platforms//isa:xtensa_lx6": "//cc/targets/esp32_idf:auto",
        "@bzd_platforms//isa:xtensa_lx7": "//cc/targets/esp32s3_idf:auto",
    }),
    visibility = ["//visibility:public"],
)
