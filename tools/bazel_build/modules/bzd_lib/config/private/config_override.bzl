"""Configuration override rules."""

load("//config:private/common.bzl", "ConfigInfo", "ConfigOverrideInfo", "label_to_key", "override_targets")
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
    if len(ctx.attr.configs) != len(ctx.attr.targets):
        fail("The number of configs must match the number of targets.")

    files = {}
    runfiles = {}
    for config, target in zip(ctx.attr.configs, ctx.attr.targets):
        if ConfigInfo in target:
            files[config] = target[ConfigInfo].json
            runfiles[config] = target[ConfigInfo].runfiles
        elif DefaultInfo in target:
            target_files = target[DefaultInfo].files.to_list()
            if len(target_files) != 1:
                fail("There must be exactly 1 file associated with the target {} for overriding config '{}', not {}.".format(target, config, len(target_files)))
            files[config] = target_files[0]
        else:
            fail("Unsupported target {} for overriding config '{}'.".format(target, config))

    return [ConfigOverrideInfo(
        files = files,
        runfiles = runfiles,
    )]

_bzd_config_override = rule(
    doc = "Apply a configuration to its dependencies.",
    implementation = _bzd_config_override_impl,
    attrs = {
        "configs": attr.string_list(
            doc = "Configurations targets to be updated.",
            mandatory = True,
        ),
        "targets": attr.label_list(
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
        targets = configs.values(),
        **kwargs
    )

# ---- Transitions ----

def _bzd_transition_config_override_impl(settings, attr):
    if not hasattr(attr, "config_override"):
        fail("This rule does not contain a config_override attribute.")
    if not attr.config_override:
        return {}

    # Check that there are remaining spots.
    if settings[override_targets[-1]] != Label("//config:empty"):
        fail("Override conflict for {}, cannot override more than {} values:\n{}".format(
            attr.target,
            len(override_targets),
            "\n".join(["- " + str(settings[t]) for t in override_targets] + ["- " + str(attr.config_override)]),
        ))

    # Shift the overrides and set the new one.
    output = {k: settings[target] for k, target in zip(override_targets[1:], override_targets)}
    output[override_targets[0]] = str(attr.config_override)

    return output

_bzd_transition_config_override = transition(
    implementation = _bzd_transition_config_override_impl,
    inputs = override_targets,
    outputs = override_targets,
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
            "config_override": attr.label(
                doc = "Configuration to be overridden.",
                providers = [ConfigOverrideInfo],
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
