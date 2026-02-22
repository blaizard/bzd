load("@rules_cc//cc:defs.bzl", "cc_library")
load("@rules_rust//rust:defs.bzl", "rust_library", "rust_library_group")

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
            "@crates_esp32//:esp-println",
        ],
    }) + [
        ":crate_linker",
    ],
)

# Create an empty library only to propagate the -Tlinkall.x flag.
rust_library(
    name = "crate_linker",
    srcs = ["lib.rs"],
    deps = [":linker"],
)

cc_library(
    name = "linker",
    linkopts = [
        "-Tlinkall.x",
        # The -Wl,--no-relax option is required to prevent the linker from relaxing the jump instructions,
        # which can cause segfault with the linker.
        "-Wl,--no-relax",
    ],
)
