"""Rust rule wrapper."""

load("@bzd_lib//:defs.bzl", "bzd_runner_factory")
load("@rules_rust//rust:defs.bzl", "rust_common", rust_binary_original = "rust_binary", rust_test_original = "rust_test")

_bzd_runner_rust_binary = bzd_runner_factory(is_test = False, providers = [rust_common.crate_info, rust_common.test_crate_info])
_bzd_runner_rust_test = bzd_runner_factory(is_test = True, providers = [rust_common.crate_info, rust_common.test_crate_info])

def _rust_binary_impl(name, visibility, crate_name, tags, **kwargs):
    rust_binary_original(
        name = "{}.original".format(name),
        crate_name = crate_name or name,
        tags = tags,
        **kwargs
    )
    _bzd_runner_rust_binary(
        name = name,
        binary = ":{}.original".format(name),
        visibility = visibility,
        tags = tags,
    )

rust_binary = macro(
    inherit_attrs = rust_binary_original,
    implementation = _rust_binary_impl,
)

def _rust_test_impl(name, visibility, crate_name, deps, tags, **kwargs):
    rust_test_original(
        name = "{}.original".format(name),
        crate_name = crate_name or name,
        use_libtest_harness = False,
        tags = (tags or []) + ["manual"],
        deps = (deps or []) + [
            "//rust/targets:main",
            "//rust/libs/bzd_test",
        ],
        **kwargs
    )
    _bzd_runner_rust_test(
        name = name,
        binary = ":{}.original".format(name),
        visibility = visibility,
        tags = tags,
    )

rust_test = macro(
    inherit_attrs = rust_test_original,
    attrs = {
        "use_libtest_harness": None,
    },
    implementation = _rust_test_impl,
)
