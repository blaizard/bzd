"""Configuration override rules."""

load("//config:private/common.bzl", "ConfigInfo", "ConfigOverrideInfo", "label_to_key")
load("//lib:attrs.bzl", "ATTRS_COMMON_BUILD_RULES", "attrs_assert_any_of")

def _bzd_config_override_impl(ctx):
    if len(ctx.attr.configs) != len(ctx.files.files):
        fail("Some files are not actual files.")

    files = {target: f for target, f in zip(ctx.attr.configs, ctx.files.files)}
    return [ConfigOverrideInfo(
        files = files,
    )]

_bzd_config_override = rule(
    doc = "Apply a configuration to its dependencies.",
    implementation = _bzd_config_override_impl,
    attrs = {
        "configs": attr.string_list(
            doc = "Configurations targets to be updated.",
            mandatory = True,
        ),
        "files": attr.label_list(
            doc = "Configurations values to be applied.",
            mandatory = True,
            allow_files = True,
        ),
    },
    provides = [ConfigOverrideInfo],
)

def _bzd_config_override_check_impl(ctx):
    return [DefaultInfo(
        files = depset([config[ConfigInfo].json for config in ctx.attr.configs]),
    )]

_bzd_config_override_check = rule(
    doc = "Checks that the config is pointing to a bzd_config rule.",
    implementation = _bzd_config_override_check_impl,
    attrs = {
        "configs": attr.label_list(
            doc = "Configurations targets to be updated.",
            providers = [ConfigInfo],
            mandatory = True,
        ),
    },
)

def bzd_config_override(name, configs, **kwargs):
    """Override a configuration and provide a target that can be used with //config:override.

    Args:
        name: The name of the target.
        configs: A dictionary which keys are the ConfigInfo targets and values are the value files.
        **kwargs: extra common build rules attributes.
    """

    attrs_assert_any_of(kwargs, ATTRS_COMMON_BUILD_RULES)

    _bzd_config_override_check(
        name = "{}.check".format(name),
        configs = configs.keys(),
        visibility = ["//visibility:private"],
    )

    _bzd_config_override(
        name = name,
        configs = [label_to_key(native.package_relative_label(config)) for config in configs.keys()],
        files = configs.values(),
        **kwargs
    )

# ---- Transitions ----

def _bzd_transition_config_override_impl(_settings, attr):
    if not hasattr(attr, "config_override"):
        fail("This rule does not contain a config_override attribute.")
    if not attr.config_override:
        return {}
    return {
        "//config:override": str(attr.config_override),
    }

bzd_transition_config_override = transition(
    implementation = _bzd_transition_config_override_impl,
    inputs = [],
    outputs = [
        "//config:override",
    ],
)
