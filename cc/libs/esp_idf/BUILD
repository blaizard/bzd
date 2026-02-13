alias(
    name = "esp_idf",
    actual = select({
        "@bzd_platforms//al_isa:esp32-xtensa_lx6": "@esp32_xtensa_lx6_sdk//:sdk",
        "@bzd_platforms//al_isa:esp32s3-xtensa_lx7": "@esp32s3_xtensa_lx7_sdk//:sdk",
    }),
    target_compatible_with = select({
        "@bzd_platforms//al_isa:esp32-xtensa_lx6": [],
        "@bzd_platforms//al_isa:esp32s3-xtensa_lx7": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    visibility = ["//visibility:public"],
)
