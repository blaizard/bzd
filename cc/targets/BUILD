alias(
    name = "auto",
    actual = select({
        "@bzd_platforms//al:linux": "//cc/targets/linux:auto",
        "@bzd_platforms//al:osx": "//cc/targets/posix:auto",
        "@bzd_platforms//isa:xtensa_lx6": "//cc/targets/esp32:auto",
        "@bzd_platforms//isa:xtensa_lx7": "//cc/targets/esp32s3:auto",
    }),
    visibility = ["//visibility:public"],
)
