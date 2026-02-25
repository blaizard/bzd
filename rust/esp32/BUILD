load("@rules_rust//rust:defs.bzl", "rust_binary", "rust_library")
load("//rust/bzd/test:defs.bzl", "rust_test")

rust_binary(
    name = "esp32",
    srcs = [
        "main.rs",
    ],
    deps = [
        "//rust/libs/esp_hal",
    ],
)

rust_library(
    name = "lib",
    srcs = [
        "lib.rs",
    ],
    deps = [
        "//rust/bzd",
        "//rust/libs/esp_hal",
        "//rust/targets:main",
    ],
)

rust_test(
    name = "test",
    crate = "lib",
)
