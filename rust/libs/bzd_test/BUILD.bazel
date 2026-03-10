load("@rules_rust//rust:defs.bzl", "rust_library", "rust_proc_macro")

rust_proc_macro(
    name = "proc_macro",
    srcs = [
        "proc_macro.rs",
    ],
    target_compatible_with = select({
        "@bzd_platforms//al:linux": [],
        "@bzd_platforms//al:osx": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    deps = [
        "@crates//:quote",
        "@crates//:syn",
    ],
)

rust_library(
    name = "bzd_test",
    srcs = [
        "assertions.rs",
        "lib.rs",
        "types.rs",
    ],
    proc_macro_deps = [
        ":proc_macro",
    ],
    visibility = ["//visibility:public"],
)
