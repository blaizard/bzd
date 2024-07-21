"""Configuration rules."""

load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")
load("//config:private/common.bzl", "ConfigInfo", "ConfigOverrideInfo", "label_to_key")
load("//lib:attrs.bzl", "ATTRS_COMMON_BUILD_RULES", "attrs_assert_any_of")

def _bzd_config_flag_impl(ctx):
    return BuildSettingInfo(value = ctx.build_setting_value)

_bzd_config_flag = rule(
    implementation = _bzd_config_flag_impl,
    build_setting = config.string(flag = True, allow_multiple = True),
)

def _bzd_config_impl(ctx):
    key_values = [keyValue for keyValue in ctx.attr.set_flag[BuildSettingInfo].value if keyValue]

    input_files = [] + ctx.files.srcs
    override_files = [] + ctx.files.file_flag
    workspace_status_files = []

    # If there are inline values, create an additional input file.
    if ctx.attr.values:
        values_file = ctx.actions.declare_file(ctx.label.name + ".values.json")
        ctx.actions.write(
            output = values_file,
            content = json.encode(ctx.attr.values),
        )
        input_files.append(values_file)

    if ctx.attr.include_workspace_status:
        workspace_status_files.append(ctx.info_file)
        workspace_status_files.append(ctx.version_file)

    if ConfigOverrideInfo in ctx.attr._override_flag:
        overrides = ctx.attr._override_flag[ConfigOverrideInfo].files
        label_key = label_to_key(ctx.label)
        if label_key in overrides:
            override_files.append(overrides[label_key])

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

    return [
        ConfigInfo(json = ctx.outputs.output_json),
        DefaultInfo(files = depset([ctx.outputs.output_json]), runfiles = ctx.runfiles(files = [output_py])),
        PyInfo(
            transitive_sources = depset([output_py]),
        ),
    ]

_bzd_config = rule(
    implementation = _bzd_config_impl,
    attrs = {
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
        "_override_flag": attr.label(
            doc = "Override the configuration if needed.",
            default = Label("//config:override"),
        ),
        "_template_py": attr.label(
            default = Label("//config:template_py.btl"),
            allow_single_file = True,
        ),
    },
    provides = [DefaultInfo, ConfigInfo, PyInfo],
)

def bzd_config(name, srcs = None, values = None, include_workspace_status = None, **kwargs):
    """Create a default configuration.

    Args:
        name: The name of the rule.
        srcs: Configuration files to be used to update the values.
        values: Inline configuration values.
        include_workspace_status: Include workspace status data.
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
        srcs = srcs or [],
        values = values or {},
        include_workspace_status = include_workspace_status or [],
        **kwargs
    )
