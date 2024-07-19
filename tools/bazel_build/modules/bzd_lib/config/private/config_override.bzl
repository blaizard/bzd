"""Configuration override rules."""

load("//config:private/common.bzl", "ConfigInfo", "ConfigOverrideInfo", "label_to_key")
load("//lib:attrs.bzl", "ATTRS_COMMON_BUILD_RULES", "attrs_assert_any_of")

# This list was taken from here: https://github.com/fmeum/with_cfg.bzl/blob/main/with_cfg/private/rule_defaults.bzl
_DEFAULT_PROVIDERS = [
    AnalysisTestResultInfo,
    CcInfo,
    CcToolchainConfigInfo,
    DebugPackageInfo,
    DefaultInfo,
    JavaInfo,
    JavaPluginInfo,
    OutputGroupInfo,
    PyInfo,
    PyRuntimeInfo,
    apple_common.AppleDebugOutputs,
    apple_common.AppleDynamicFramework,
    apple_common.AppleExecutableBinary,
    apple_common.Objc,
    apple_common.XcodeProperties,
    apple_common.XcodeVersionConfig,
    config_common.FeatureFlagInfo,
    java_common.BootClassPathInfo,
    java_common.JavaRuntimeInfo,
    java_common.JavaToolchainInfo,
    platform_common.TemplateVariableInfo,
    platform_common.ToolchainInfo,
]

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

def _bzd_config_override_macro(name, configs, **kwargs):
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

_bzd_transition_config_override = transition(
    implementation = _bzd_transition_config_override_impl,
    inputs = [],
    outputs = [
        "//config:override",
    ],
)

# ---- Factory ----

def make_bzd_config_apply(name = None, providers = None, executable = False, test = False):
    """Factory function to create a bzd_config_apply, to apply a configuration to a target.

    Args:
        name: Ignored argument to make sanitizer happy.
        providers: A list of providers to be forwarded from the target.
        executable: If the rule is executable or not.
        test: If the rule is a test or not.

    Returns:
        A tuple containing the macro to be used and an internal rule, the latter should not be used.
    """

    def _bzd_config_apply_impl(ctx):
        # Due to the transition, the target becomes an array.
        target = ctx.attr.target[0]
        all_providers = [target[provider] for provider in (_DEFAULT_PROVIDERS + (providers or [])) if provider in target]
        return all_providers

    _bzd_config_apply_rule = rule(
        doc = "Apply a specific configuration to a target.",
        implementation = _bzd_config_apply_impl,
        attrs = {
            "config_override": attr.label(
                doc = "Configuration to be overridden.",
                providers = [ConfigOverrideInfo],
            ),
            "target": attr.label(
                mandatory = True,
                doc = "The target associated with this application.",
                cfg = _bzd_transition_config_override,
            ),
        },
        executable = executable,
        test = test,
    )

    def _bzd_config_apply_macro(name, configs, **kwargs):
        """Apply a configuration to the specified target.

        Args:
            name: The name of the target.
            configs: The configuration to be applied.
            **kwargs: Extra arguments to be propagated to the rule.
        """

        _bzd_config_override_macro(
            name = "{}.config_override".format(name),
            configs = configs,
            tags = ["manual"],
            visibility = ["//visibility:private"],
        )

        _bzd_config_apply_rule(
            name = name,
            config_override = "{}.config_override".format(name),
            **kwargs
        )

    return _bzd_config_apply_macro, _bzd_config_apply_rule

# ---- Rules ----

bzd_config_apply, _library = make_bzd_config_apply()
bzd_config_apply_binary, _binary = make_bzd_config_apply(executable = True)
bzd_config_apply_test, _test = make_bzd_config_apply(executable = True, test = True)
