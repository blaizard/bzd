load("@rules_rust//rust:defs.bzl", "rust_library")

rust_library(
    name = "main",
    srcs = [
        "main.rs",
    ],
    rustc_flags = [
        "-C",
        "link-arg=-Wl,--undefined=main",
        "-C",
        "link-arg=-Wl,--undefined=rust_begin_unwind",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//rust/libs/esp_hal",
    ],
    alwayslink = True,
)
