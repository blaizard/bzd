alias(
    name = "main",
    actual = select({
        "@bzd_platforms//al:linux": "//rust/targets/std:main",
        "@bzd_platforms//al:osx": "//rust/targets/std:main",
        "@bzd_platforms//al_isa:esp32-xtensa_lx6": "//rust/targets/esp:main",
        "@bzd_platforms//al_isa:esp32s3-xtensa_lx7": "//rust/targets/esp:main",
    }),
    visibility = ["//visibility:public"],
)
