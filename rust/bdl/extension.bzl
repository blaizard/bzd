"""Rust extension for bdl rules."""

load("@rules_rust//rust:defs.bzl", "rust_common")
load("//rust/bdl/generator:composition.bzl", "generator_rust_composition")
load("//rust/bdl/generator:library.bzl", "generator_rust_library")

extension = {
    "composition": {
        "generator": generator_rust_composition,
    },
    "library": {
        "generator": generator_rust_library,
        "providers": [rust_common.crate_info, rust_common.dep_info],
    },
}
