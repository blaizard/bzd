"""Configuration rules."""

load("//config:private/config.bzl", _bzd_config = "bzd_config")
load(
    "//config:private/config_override.bzl",
    _bzd_config_apply = "bzd_config_apply",
    _bzd_config_apply_binary = "bzd_config_apply_binary",
    _bzd_config_apply_test = "bzd_config_apply_test",
    _make_bzd_config_apply = "make_bzd_config_apply",
)

bzd_config = _bzd_config
bzd_config_apply = _bzd_config_apply
bzd_config_apply_binary = _bzd_config_apply_binary
bzd_config_apply_test = _bzd_config_apply_test
make_bzd_config_apply = _make_bzd_config_apply
