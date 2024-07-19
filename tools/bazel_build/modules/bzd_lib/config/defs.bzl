"""Configuration rules."""

load("//config:private/common.bzl", _ConfigOverrideInfo = "ConfigOverrideInfo")
load("//config:private/config.bzl", _bzd_config = "bzd_config")
load(
    "//config:private/config_override.bzl",
    _bzd_config_apply = "bzd_config_apply",
    _bzd_config_apply_binary = "bzd_config_apply_binary",
    _bzd_config_apply_test = "bzd_config_apply_test",
    _bzd_config_override = "bzd_config_override",
    _bzd_transition_config_override = "bzd_transition_config_override",
    _make_bzd_config_apply = "make_bzd_config_apply",
)

ConfigOverrideInfo = _ConfigOverrideInfo
bzd_config = _bzd_config
bzd_config_override = _bzd_config_override
bzd_transition_config_override = _bzd_transition_config_override
make_bzd_config_apply = _make_bzd_config_apply
bzd_config_apply = _bzd_config_apply
bzd_config_apply_binary = _bzd_config_apply_binary
bzd_config_apply_test = _bzd_config_apply_test
