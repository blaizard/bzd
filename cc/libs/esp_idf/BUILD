alias(
    name = "esp_idf",
    actual = select({
        "@bzd_platforms//al_isa:esp32_idf-xtensa_lx6": "@esp32_xtensa_lx6_sdk//:sdk",
        "@bzd_platforms//al_isa:esp32s3_idf-xtensa_lx7": "@esp32s3_xtensa_lx7_sdk//:sdk",
        "//conditions:default": "@platforms//:incompatible",
    }),
    visibility = ["//visibility:public"],
)
