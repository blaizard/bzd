load("@rules_rust//rust:defs.bzl", "rust_library_group")

rust_library_group(
    name = "esp_hal",
    target_compatible_with = select({
        "@bzd_platforms//al_isa:esp32-xtensa_lx6": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    visibility = ["//visibility:public"],
    deps = select({
        "@bzd_platforms//al_isa:esp32-xtensa_lx6": [
            "@crates_esp32//:esp-backtrace",
            "@crates_esp32//:esp-hal",
        ],
    }),
)
