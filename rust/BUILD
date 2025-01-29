load("@rules_rust//rust:defs.bzl", "rust_binary", "rust_library")

rust_binary(
    name = "hello_world",
    srcs = ["hello_world.rs"],
    deps = [
        ":non_owning_ring_spin",
    ],
)

rust_library(
    name = "non_owning_ring_spin",
    srcs = ["non_owning_ring_spin.rs"],
)
