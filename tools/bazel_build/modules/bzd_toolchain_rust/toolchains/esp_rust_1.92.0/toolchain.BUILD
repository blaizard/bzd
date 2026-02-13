load("@rules_rust//rust:toolchain.bzl", "rust_stdlib_filegroup")

package(default_visibility =  ["//visibility:public"])

alias(
	name = "rustc",
	actual = "rustc/bin/rustc",
)

alias(
	name = "rustdoc",
	actual = "rustc/bin/rustdoc",
)

rust_stdlib_filegroup(
    name = "stdlib_exec",
    srcs = glob(
        [
			"rust-std-x86_64-unknown-linux-gnu/lib/rustlib/x86_64-unknown-linux-gnu/lib/*.rlib",
            "rust-std-x86_64-unknown-linux-gnu/lib/rustlib/x86_64-unknown-linux-gnu/lib/*.so*",
            "rust-std-x86_64-unknown-linux-gnu/lib/rustlib/x86_64-unknown-linux-gnu/lib/*.a",
            "rust-std-x86_64-unknown-linux-gnu/lib/rustlib/x86_64-unknown-linux-gnu/lib/self-contained/**",
        ],
        # Some patterns (e.g. `lib/*.a`) don't match anything, see https://github.com/bazelbuild/rules_rust/pull/245
        allow_empty = True,
    ),
)
