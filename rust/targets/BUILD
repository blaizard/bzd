alias(
    name = "main",
    actual = select({
        "@bzd_platforms//al:linux": "//rust/targets/std:main",
        "@bzd_platforms//al:osx": "//rust/targets/std:main",
        "@bzd_platforms//isa:xtensa_lx6": "//rust/targets/esp32:main",
        "@bzd_platforms//isa:xtensa_lx7": "//rust/targets/esp32:main",
    }),
    visibility = ["//visibility:public"],
)
