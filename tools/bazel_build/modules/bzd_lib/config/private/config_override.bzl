"""Configuration override rules."""

load("@config_defaults//:defs.bzl", "CONFIG_DEFAULTS")
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

# ---- Transitions ----

def _bzd_transition_config_override_impl(settings, attr):
    for key in ["config_overrides", "config_values"]:
        if not hasattr(attr, key):
            fail("The rule '{}' is used as a transition but does not contain a '{}' attribute.".format(attr, key))

    if len(attr.config_overrides) != len(attr.config_values):
        fail("There must be as many config_overrides as config_values: {} vs {}".format(len(attr.config_overrides), len(attr.config_values)))

    # Make a copy.
    outputs = dict(settings.items())

    # Apply the transitions and do some checks.
    for override, value in zip(attr.config_overrides, attr.config_values):
        label = override.same_package_label("{}.file".format(override.name))
        label_str = str(label)
        if label_str not in CONFIG_DEFAULTS:
            fail(
                "The config target '{}' is not registered as a valid override, only the following are:\n{}\nPlease register the default config with the extension.".format(
                    label_str,
                    ",\n".join(["- " + s for s in CONFIG_DEFAULTS]),
                ),
            )
        if outputs[label_str] != Label("//config:empty"):
            fail("The configuration '{}' is overwritten twice.".format(label_str))
        outputs[label_str] = value

    return outputs

_bzd_transition_config_override = transition(
    implementation = _bzd_transition_config_override_impl,
    inputs = CONFIG_DEFAULTS,
    outputs = CONFIG_DEFAULTS,
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
        ignore_providers = []
        extra_providers = []

        # An executable rule must create its executable.
        if executable:
            ctx.actions.symlink(
                output = ctx.outputs.executable,
                target_file = ctx.executable.target,
                is_executable = True,
            )
            default_info_provider = DefaultInfo(
                executable = ctx.outputs.executable,
                runfiles = target[DefaultInfo].default_runfiles,
            )
            ignore_providers.append(DefaultInfo)
            extra_providers.append(default_info_provider)

        all_providers = [target[provider] for provider in (_DEFAULT_PROVIDERS + (providers or [])) if (provider in target) and (provider not in ignore_providers)]
        return all_providers + extra_providers

    _bzd_config_apply_rule = rule(
        doc = "Apply a specific configuration to a target.",
        implementation = _bzd_config_apply_impl,
        attrs = {
            "config_overrides": attr.label_list(
                doc = "Configuration to be overritten.",
                mandatory = True,
            ),
            "config_values": attr.label_list(
                doc = "Configurations values to be applied.",
                mandatory = True,
                allow_files = True,
            ),
            "target": attr.label(
                mandatory = True,
                executable = executable,
                doc = "The target associated with this application.",
                cfg = _bzd_transition_config_override,
            ),
        },
        executable = executable,
        test = test,
    )

    def _bzd_config_apply_macro(name, target, configs, **kwargs):
        """Apply a configuration to the specified target.

        Args:
            name: The name of the target.
            target: The target associated with this application.
            configs: The configuration to be applied.
            **kwargs: Extra arguments to be propagated to the rule.
        """

        attrs_assert_any_of(kwargs, ATTRS_COMMON_BUILD_RULES)

        _bzd_config_apply_rule(
            name = name,
            target = target,
            config_overrides = configs.keys(),
            config_values = configs.values(),
            **kwargs
        )

    return _bzd_config_apply_macro, _bzd_config_apply_rule

# ---- Rules ----

bzd_config_apply, _library = make_bzd_config_apply()
bzd_config_apply_binary, _binary = make_bzd_config_apply(executable = True)
bzd_config_apply_test, _test = make_bzd_config_apply(executable = True, test = True)
