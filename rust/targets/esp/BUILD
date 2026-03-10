load("@rules_rust//rust:defs.bzl", "rust_library")

rust_library(
    name = "main",
    srcs = [
        "main.rs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//rust/bzd",
        "@bzd_espressif//hal/rust",
    ],
    alwayslink = True,
)
