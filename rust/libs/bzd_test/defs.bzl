"""Test helper."""

load("@rules_rust//rust:defs.bzl", _original_rust_test = "rust_test")

def rust_test(name, deps = None, **kwargs):
    _original_rust_test(
        name = name,
        use_libtest_harness = False,
        deps = (deps or []) + [
            "//rust/targets:main",
            "//rust/libs/bzd_test",
        ],
        **kwargs
    )
