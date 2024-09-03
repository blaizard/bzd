"""Configuration rules."""

load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")
load("//config:private/common.bzl", "ConfigInfo", "ConfigOverrideInfo", "label_to_key", "override_targets")
load("//lib:attrs.bzl", "ATTRS_COMMON_BUILD_RULES", "attrs_assert_any_of")

def _bzd_config_flag_impl(ctx):
    return BuildSettingInfo(value = ctx.build_setting_value)

_bzd_config_flag = rule(
    implementation = _bzd_config_flag_impl,
    build_setting = config.string(flag = True, allow_multiple = True),
)

def _target_to_runfiles(ctx, target):
    """Get the runfiles associated with a target."""

    if type(target) == "File":
        return ctx.runfiles(
            files = [target],
        )

    elif target.files_to_run.executable:
        return ctx.runfiles().merge_all([ctx.runfiles(
            files = [target.files_to_run.executable],
        ), target.default_runfiles])

    else:
        return target.default_runfiles

def _bzd_config_impl(ctx):
    key_values = [keyValue for keyValue in ctx.attr.set_flag[BuildSettingInfo].value if keyValue] if ctx.attr.set_flag else []

    input_files = [] + ctx.files.srcs
    override_files = [] + ctx.files.file_flag
    workspace_status_files = []
    runfiles = ctx.runfiles().merge_all([_target_to_runfiles(ctx, target) for target in ctx.attr.data])

    # If there are inline values, create an additional input file.
    if ctx.attr.values:
        values_file = ctx.actions.declare_file(ctx.label.name + ".values.json")
        expanded_values = {}
        for key, value in ctx.attr.values.items():
            expanded_values[key] = ctx.expand_location(value, targets = ctx.attr.data)
        ctx.actions.write(
            output = values_file,
            content = json.encode(expanded_values),
        )
        input_files.append(values_file)

    if ctx.attr.include_workspace_status:
        workspace_status_files.append(ctx.info_file)
        workspace_status_files.append(ctx.version_file)

    # Override config and runfiles from ConfigOverrideInfo.
    for override_flag in ctx.attr.override_flag_list:
        if ConfigOverrideInfo in override_flag:
            label_key = label_to_key(ctx.label)
            config_override_files = override_flag[ConfigOverrideInfo].files
            if label_key in config_override_files:
                override_files.append(config_override_files[label_key])
            config_override_runfiles = override_flag[ConfigOverrideInfo].runfiles
            if label_key in config_override_runfiles:
                runfiles = runfiles.merge(config_override_runfiles[label_key])

    # Build the default configuration.
    ctx.actions.run(
        inputs = input_files + override_files + workspace_status_files,
        outputs = [ctx.outputs.output_json],
        progress_message = "Generating default configuration for {}...".format(ctx.label),
        arguments = [
                        "--fail-on-conflict",
                        "--output",
                        ctx.outputs.output_json.path,
                    ] +
                    ["--set=" + key_value for key_value in key_values] +
                    ["--file=" + f.path for f in override_files] +
                    ["--workspace-status-file=" + f.path for f in workspace_status_files] +
                    ["--workspace-status-key=" + key for key in ctx.attr.include_workspace_status] +
                    [f.path for f in input_files],
        executable = ctx.executable._config_merge,
    )

    # Create a python output
    output_py = ctx.actions.declare_file(ctx.label.name + ".py")
    ctx.actions.run(
        inputs = [ctx.outputs.output_json, ctx.file._template_py],
        outputs = [output_py],
        arguments = [
            "--output",
            output_py.path,
            "--json",
            ctx.outputs.output_json.path,
            ctx.file._template_py.path,
        ],
        executable = ctx.executable._config_template,
    )

    # Needed for py_binary to find the file.
    runfiles = runfiles.merge(ctx.runfiles(files = [output_py]))

    return [
        ConfigInfo(json = ctx.outputs.output_json, runfiles = runfiles),
        DefaultInfo(runfiles = runfiles),
        PyInfo(
            transitive_sources = depset([output_py]),
        ),
    ]

_bzd_config = rule(
    implementation = _bzd_config_impl,
    attrs = {
        "data": attr.label_list(
            doc = "Dependencies for this rule, will be added in the runfiles and used for target expansion.",
        ),
        "file_flag": attr.label(
            doc = "Build settings to modify the configuration using files.",
            allow_files = True,
        ),
        "include_workspace_status": attr.string_list(
            doc = "Include the key/value pairs from the status information about the workspace, see --workspace_status_command.",
        ),
        "output_json": attr.output(
            doc = "Create a json configuration.",
        ),
        "override_flag_list": attr.label_list(
            doc = "Override the configuration if needed with these flags.",
        ),
        "set_flag": attr.label(
            doc = "Build settings to modify the configuration using key/value pair.",
            providers = [BuildSettingInfo],
        ),
        "srcs": attr.label_list(
            allow_files = [".json"],
            doc = "Configuration files.",
        ),
        "values": attr.string_dict(
            doc = "Inline configuration values.",
        ),
        "_config_merge": attr.label(
            default = Label("//config:merge"),
            cfg = "exec",
            executable = True,
        ),
        "_config_template": attr.label(
            default = Label("//config:template"),
            cfg = "exec",
            executable = True,
        ),
        "_template_py": attr.label(
            default = Label("//config:template_py.btl"),
            allow_single_file = True,
        ),
    },
    provides = [DefaultInfo, ConfigInfo, PyInfo],
)

def bzd_config(name, srcs = None, values = None, include_workspace_status = None, data = None, **kwargs):
    """Create a configuration that cannot be overwritten.

    Args:
        name: The name of the rule.
        srcs: Configuration files to be used to update the values.
        values: Inline configuration values.
        include_workspace_status: Include workspace status data.
        data: Extra dependencies to be added to the runfiles.
        **kwargs: extra common build rules attributes.
    """

    attrs_assert_any_of(kwargs, ATTRS_COMMON_BUILD_RULES)

    _bzd_config(
        name = name,
        output_json = "{}.json".format(name),
        srcs = srcs or [],
        values = values or {},
        include_workspace_status = include_workspace_status or [],
        data = data,
        **kwargs
    )

def bzd_config_default(name, srcs = None, values = None, include_workspace_status = None, data = None, **kwargs):
    """Create a default configuration that can be overwritten.

    Args:
        name: The name of the rule.
        srcs: Configuration files to be used to update the values.
        values: Inline configuration values.
        include_workspace_status: Include workspace status data.
        data: Extra dependencies to be added to the runfiles.
        **kwargs: extra common build rules attributes.
    """

    attrs_assert_any_of(kwargs, ATTRS_COMMON_BUILD_RULES)

    _bzd_config_flag(
        name = name + ".set",
        build_setting_default = "",
        visibility = ["//visibility:private"],
    )

    native.label_flag(
        name = name + ".file",
        build_setting_default = "@bzd_lib//config:empty",
        visibility = ["//visibility:private"],
    )

    _bzd_config(
        name = name,
        set_flag = ":{}.set".format(name),
        file_flag = ":{}.file".format(name),
        output_json = "{}.json".format(name),
        override_flag_list = [Label(target) for target in override_targets],
        srcs = srcs or [],
        values = values or {},
        include_workspace_status = include_workspace_status or [],
        data = data,
        **kwargs
    )
