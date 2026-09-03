"""Configuration rules."""

load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")
load("//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")
load("//config:private/common.bzl", "ConfigInfo", "ConfigSourceInfo")

def _get_file_from_target(attr):
    """Get the file from a target along with its associated runfiles and data."""

    if ConfigInfo in attr:
        return [attr[ConfigInfo].internal, attr[ConfigInfo].runfiles, attr[ConfigInfo].data]
    elif attr[DefaultInfo].files:
        all_files = attr[DefaultInfo].files.to_list()
        if len(all_files) != 1:
            fail("There must be exactly 1 config: {}".format(attr.label))
        return [all_files[0], None, None]
    else:
        fail("Invalid config override target type: {}".format(attr.label))

def _update_from_target(attr, runfiles, data):
    [file, attr_runfiles, attr_data] = _get_file_from_target(attr)
    if attr_runfiles:
        runfiles = runfiles.merge(attr_runfiles)
    if attr_data:
        data = depset(transitive = [data, attr_data])
    return [file, runfiles, data]

def _bzd_config_flag_impl(ctx):
    return BuildSettingInfo(value = ctx.build_setting_value)

_bzd_config_flag = rule(
    implementation = _bzd_config_flag_impl,
    build_setting = config.string(flag = True, allow_multiple = True),
)

def _bzd_config_impl(ctx):
    input_files = []
    runfiles = ctx.runfiles(files = ctx.files.data).merge_all([target.default_runfiles for target in ctx.attr.data])
    data = depset(ctx.files.data)

    args = ctx.actions.args()

    # Handle files.
    for attr in [ctx.attr.base] + ctx.attr.srcs:
        if attr:
            [file, runfiles, data] = _update_from_target(attr, runfiles, data)
            input_files.append(file)
            args.add("--src", file)

    # Handle files at a given key.
    if ctx.attr.srcs_at:
        for key, target in ctx.attr.srcs_at.items():
            if ConfigSourceInfo in target:
                if target[ConfigSourceInfo].file:
                    args.add("--src-at", json.encode([key, target[ConfigSourceInfo].file.path, target[ConfigSourceInfo].metadata]))
                    input_files.append(target[ConfigSourceInfo].file)
                elif target[ConfigSourceInfo].content:
                    args.add("--value", json.encode([key, target[ConfigSourceInfo].content, target[ConfigSourceInfo].metadata]))
                else:
                    fail("'ConfigSourceInfo' must have either 'file' or 'content' set.")
            else:
                [file, runfiles, data] = _update_from_target(target, runfiles, data)
                args.add("--src-at", json.encode([key, file.path, []]))
                input_files.append(file)

    # Handle inline values.
    for key, value in ctx.attr.values.items():
        metadata = []
        if value.startswith("$(path "):
            value = value.replace("$(path ", "$(rlocationpath ", 1)
            metadata.append("path")
        args.add("--value", json.encode([key, ctx.expand_location(value, targets = ctx.attr.data), metadata]))

    if ctx.attr.include_workspace_status:
        workspace_status_files = [
            ctx.info_file,
            ctx.version_file,
        ]
        args.add_all(ctx.attr.include_workspace_status, before_each = "--workspace-status-key")
        args.add_all(workspace_status_files, before_each = "--workspace-status-file")
        input_files += workspace_status_files

    # Build the default configuration.
    internal = ctx.actions.declare_file("{}.internal".format(ctx.label))
    args.add("--output", internal)
    ctx.actions.run(
        inputs = depset(input_files + ctx.files.deps),
        outputs = [internal],
        progress_message = "Generating default configuration for {}...".format(ctx.label),
        arguments = [args],
        executable = ctx.executable._config_merge,
    )

    return [
        ConfigInfo(internal = internal, runfiles = runfiles, data = data),
        DefaultInfo(runfiles = runfiles, files = data),
    ]

_bzd_config = rule(
    implementation = _bzd_config_impl,
    doc = "Create a configuration from given inputs.",
    attrs = {
        "base": attr.label(
            doc = "Base configuration to update.",
        ),
        "data": attr.label_list(
            doc = "Dependencies for this rule, will be added in the runfiles and used for target expansion.",
            allow_files = True,
        ),
        "deps": attr.label_list(
            doc = "Build dependencies for this rule, will be used when building the configuration in the execution target environment.",
            allow_files = [".json", ".yaml", ".yml"],
        ),
        "include_workspace_status": attr.string_list(
            doc = "Include the key/value pairs from the status information about the workspace, see --workspace_status_command.",
        ),
        "srcs": attr.label_list(
            allow_files = [".json", ".yaml", ".yml"],
            doc = "Configuration files.",
        ),
        "srcs_at": attr.string_keyed_label_dict(
            allow_files = True,
            doc = """Configuration files that will be merged at the specified key.""",
        ),
        "values": attr.string_dict(
            doc = "Inline configuration values.",
        ),
        "_config_merge": attr.label(
            default = Label("//config:merge"),
            cfg = "exec",
            executable = True,
        ),
    },
    provides = [DefaultInfo, ConfigInfo],
)

def _bzd_config_update_impl(ctx):
    args = ctx.actions.args()

    [file, runfiles, data] = _get_file_from_target(ctx.attr.base)
    args.add("--src", file)

    for keyValue in (ctx.attr.set_flag[BuildSettingInfo].value if ctx.attr.set_flag else []):
        if not keyValue:
            continue
        args.add("--override-set", keyValue)

    # Build the updated configuration.
    internal = ctx.actions.declare_file("{}.internal".format(ctx.label))
    args.add("--output", internal)
    ctx.actions.run(
        inputs = [file],
        outputs = [internal],
        progress_message = "Generating default configuration for {}...".format(ctx.label),
        arguments = [args],
        executable = ctx.executable._config_merge,
    )

    return [
        ConfigInfo(internal = internal, runfiles = runfiles, data = data),
    ]

_bzd_config_update = rule(
    implementation = _bzd_config_update_impl,
    doc = "Update an existing configuration, this is needed to support overrides.",
    attrs = {
        "base": attr.label(
            allow_files = True,
            doc = "Base configuration to update.",
        ),
        "set_flag": attr.label(
            doc = "Build settings to modify the configuration using key/value pair, this will update the current configuration.",
            providers = [BuildSettingInfo],
        ),
        "_config_merge": attr.label(
            default = Label("//config:merge"),
            cfg = "exec",
            executable = True,
        ),
    },
    provides = [DefaultInfo, ConfigInfo],
)

def _bzd_config_convert_impl(ctx):
    config_info = ctx.attr.config[ConfigInfo]

    # Create the additional outputs
    outputs = {
        "json": ctx.outputs.output_json,
        "yaml": ctx.outputs.output_yaml,
    }
    for format, file in outputs.items():
        ctx.actions.run(
            inputs = [config_info.internal],
            outputs = [file],
            arguments = [
                "--output",
                file.path,
                "--internal",
                config_info.internal.path,
                "--format",
                format,
            ],
            progress_message = "Converting configuration to {} for {}...".format(format, ctx.label),
            executable = ctx.executable._config_convert,
        )

    return [
        config_info,
        DefaultInfo(runfiles = config_info.runfiles, files = config_info.data),
    ]

_bzd_config_convert = rule(
    implementation = _bzd_config_convert_impl,
    doc = "Convert a configuration into various formats.",
    attrs = {
        "config": attr.label(
            providers = [ConfigInfo],
            doc = "The base config to be used.",
        ),
        "output_json": attr.output(
            doc = "Create a json configuration.",
        ),
        "output_yaml": attr.output(
            doc = "Create a yaml configuration.",
        ),
        "_config_convert": attr.label(
            default = Label("//config:convert"),
            cfg = "exec",
            executable = True,
        ),
    },
    provides = [DefaultInfo, ConfigInfo],
)

def _bzd_config_viewer_impl(ctx):
    config_info = ctx.attr.config[ConfigInfo]
    return sh_binary_wrapper_impl(
        ctx = ctx,
        locations = {
            ctx.attr._config_viewer: "viewer",
            config_info.internal: "internal",
        },
        output = ctx.outputs.executable,
        command = "{viewer} --internal {internal}",
    )

_bzd_config_viewer = rule(
    implementation = _bzd_config_viewer_impl,
    doc = "Visualize the configuration.",
    attrs = {
        "config": attr.label(
            providers = [ConfigInfo],
            doc = "The base config to be used.",
        ),
        "_config_viewer": attr.label(
            default = Label("//config:viewer"),
            cfg = "target",
            executable = True,
        ),
    },
    executable = True,
)

def _bzd_config_macro_impl(name, visibility, output_json, output_yaml, **kwargs):
    _bzd_config(
        name = "{}.base".format(name),
        **kwargs
    )

    _bzd_config_convert(
        name = name,
        config = "{}.base".format(name),
        output_json = output_json if output_json else "{}.json".format(name),
        output_yaml = output_yaml if output_yaml else "{}.yaml".format(name),
        visibility = visibility,
    )

    _bzd_config_viewer(
        name = "{}.view".format(name),
        config = "{}.base".format(name),
    )

bzd_config = macro(
    doc = "Create a configuration that cannot be overwritten.",
    implementation = _bzd_config_macro_impl,
    inherit_attrs = _bzd_config,
    attrs = {
        "output_json": attr.string(
            doc = "Name of the generated json configuration.",
            configurable = False,
        ),
        "output_yaml": attr.string(
            doc = "Name of the generated yaml configuration.",
            configurable = False,
        ),
        "override_flag": None,
        "set_flag": None,
    },
)

def _bzd_config_default_macro_impl(name, visibility, output_json, output_yaml, **kwargs):
    _bzd_config(
        name = "{}.base".format(name),
        visibility = visibility,
        **kwargs
    )

    _bzd_config_flag(
        name = "{}.set".format(name),
        build_setting_default = "",
        visibility = visibility,
    )

    native.label_flag(
        name = "{}.override".format(name),
        build_setting_default = "{}.base".format(name),
        visibility = visibility,
    )

    _bzd_config_update(
        name = "{}.update".format(name),
        set_flag = ":{}.set".format(name),
        base = ":{}.override".format(name),
    )

    _bzd_config_convert(
        name = name,
        config = "{}.update".format(name),
        output_json = output_json if output_json else "{}.json".format(name),
        output_yaml = output_yaml if output_yaml else "{}.yaml".format(name),
        visibility = visibility,
    )

    _bzd_config_viewer(
        name = "{}.view".format(name),
        config = "{}.base".format(name),
    )

bzd_config_default = macro(
    doc = "Create a default configuration that can be overwritten.",
    implementation = _bzd_config_default_macro_impl,
    inherit_attrs = _bzd_config,
    attrs = {
        "output_json": attr.string(
            doc = "Name of the generated json configuration.",
            configurable = False,
        ),
        "output_yaml": attr.string(
            doc = "Name of the generated yaml configuration.",
            configurable = False,
        ),
        "override_flag": None,
        "set_flag": None,
    },
)
