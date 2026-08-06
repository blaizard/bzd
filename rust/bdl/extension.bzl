"""Rust extension for bdl rules."""

load("@rules_rust//rust:defs.bzl", "rust_common")
load("//rust/bdl/generator:library.bzl", "generator_rust_library")

extension = {
    "library": {
        "generator": generator_rust_library,
        "providers": [rust_common.crate_info, rust_common.dep_info],
    },
}
