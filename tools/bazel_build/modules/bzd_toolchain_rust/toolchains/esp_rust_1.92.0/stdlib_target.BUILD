load("@rules_rust//rust:defs.bzl", "rust_library")

package(default_visibility =  ["//visibility:public"])

filegroup(
    name = "stdlib_target",
    srcs = [
        ":compiler_builtins",
        ":core",
	],
)

rust_library(
    name = "compiler_builtins",
    srcs = glob([
        "library/compiler-builtins/compiler-builtins/**/*.rs",
        "library/compiler-builtins/libm/src/math/**/*.rs",
    ]),
    crate_features = [
        "mem-unaligned",
        "compiler-builtins",
        # "core",
        # "default",
        "unstable",
        "no_std",
        "no-asm",
    ],
    crate_name = "compiler_builtins",
    #edition = "2024",
    deps = [":core"],
    rustc_env = {
        "RUSTC_BOOTSTRAP": "1",
    },
)

rust_library(
    name = "core",
    srcs = glob([
        "library/core/src/**/*.rs",
        "library/stdarch/crates/core_arch/src/**/*.rs",
        "library/portable-simd/crates/core_simd/src/**/*.rs",
    ]),
    compile_data = glob([
        "library/core/src/**/*.md",
        "library/stdarch/crates/core_arch/src/**/*.md",
        "library/portable-simd/crates/core_simd/src/**/*.md",
    ]),
    crate_features = [
        # "stdsimd",
        # "no_std",
    ],
    crate_name = "core",
    #edition = "2024",
    rustc_flags = [
        "--cfg=bootstrap",
        "--cap-lints=allow",
        "-Zmacro-backtrace",
    ],
    rustc_env = {
        "RUSTC_BOOTSTRAP": "1",
    },
)
