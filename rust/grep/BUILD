load("@rules_rust//rust:defs.bzl", "rust_binary", "rust_library", "rust_test")

rust_library(
    name = "lib",
    srcs = [
        "lib.rs",
    ],
)

rust_test(
    name = "lib_test",
    crate = ":lib",
)

rust_binary(
    name = "grep",
    srcs = [
        "main.rs",
    ],
    deps = [
        ":lib",
    ],
)
