"""Configuration rules."""

load("//config:private/config.bzl", _bzd_config = "bzd_config", _bzd_config_default = "bzd_config_default")
load(
    "//config:private/config_override.bzl",
    _make_bzd_config_apply = "make_bzd_config_apply",
)

bzd_config = _bzd_config
bzd_config_default = _bzd_config_default
make_bzd_config_apply = _make_bzd_config_apply
