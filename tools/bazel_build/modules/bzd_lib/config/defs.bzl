"""Configuration rules."""

load("//config:private/common.bzl", _ConfigOverrideInfo = "ConfigOverrideInfo")
load("//config:private/config.bzl", _bzd_config = "bzd_config")
load("//config:private/config_override.bzl", _bzd_config_override = "bzd_config_override", _bzd_transition_config_override = "bzd_transition_config_override")

ConfigOverrideInfo = _ConfigOverrideInfo
bzd_config = _bzd_config
bzd_config_override = _bzd_config_override
bzd_transition_config_override = _bzd_transition_config_override
