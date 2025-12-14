"""Configuration rules."""

load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")
load("//config:private/common.bzl", "ConfigInfo")

def _from_attr_to_config_info(ctx, attr):
    """Read an input config attribute and convert it into a file, runfiles and data triplet."""

    if ConfigInfo in attr:
        return [attr[ConfigInfo].internal, attr[ConfigInfo].runfiles, attr[ConfigInfo].data]
    elif attr[DefaultInfo].files:
        if len(attr[DefaultInfo].files.to_list()) != 1:
            fail("There must be exactly 1 config: {}".format(attr.label))
        return [attr[DefaultInfo].files, ctx.runfiles(), depset()]
    else:
        fail("Invalid config override target type: {}".format(attr.label))

def _bzd_config_flag_impl(ctx):
    return BuildSettingInfo(value = ctx.build_setting_value)

_bzd_config_flag = rule(
    implementation = _bzd_config_flag_impl,
    build_setting = config.string(flag = True, allow_multiple = True),
)

def _bzd_config_impl(ctx):
    input_files = depset()
    runfiles = ctx.runfiles().merge_all([target.default_runfiles for target in ctx.attr.data])
    data = depset(ctx.files.data)

    args = ctx.actions.args()

    if ctx.attr.base:
        [base, runfiles_base, data_base] = _from_attr_to_config_info(ctx, ctx.attr.base)
        runfiles = runfiles.merge(runfiles_base)
        data = depset(transitive = [data, data_base])
        input_files = depset([base], transitive = [input_files])
        args.add("--base", base)

    # Handle inline values.
    for key, value in ctx.attr.values.items():
        args.add_all("--value-at", [key, ctx.expand_location(value, targets = ctx.attr.data)])

    # Handle files.
    if ctx.attr.srcs:
        args.add_all(ctx.files.srcs, before_each = "--src")
        input_files = depset(ctx.files.srcs, transitive = [input_files])

    # Handle files at a given key.
    if ctx.attr.srcs_at:
        for target, keys in ctx.attr.srcs_at.items():
            for key in keys.split(","):
                args.add_all("--src-at", [key, target.files.to_list()[0]])
        input_files = depset(ctx.files.srcs_at, transitive = [input_files])

    if ctx.attr.include_workspace_status:
        workspace_status_files = [
            ctx.info_file,
            ctx.version_file,
        ]
        args.add_all(ctx.attr.include_workspace_status, before_each = "--workspace-status-key")
        args.add_all(workspace_status_files, before_each = "--workspace-status-file")
        input_files = depset(workspace_status_files, transitive = [input_files])

    # Build the default configuration.
    internal = ctx.actions.declare_file("{}.internal".format(ctx.label))
    args.add("--output", internal)
    ctx.actions.run(
        inputs = input_files,
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
        "include_workspace_status": attr.string_list(
            doc = "Include the key/value pairs from the status information about the workspace, see --workspace_status_command.",
        ),
        "srcs": attr.label_list(
            allow_files = [".json", ".yaml", ".yml"],
            doc = "Configuration files.",
        ),
        "srcs_at": attr.label_keyed_string_dict(
            allow_files = [".json", ".yaml", ".yml"],
            doc = """Configuration files that will be merged at the specified key.
            
            The value corresponds to one of multiples keys (separated by a comma). This allow the same config to be used
            at multiple places in the configuration.
            It also allow multiple config to be applied to the same key, in that case, they will be merged together.

            Note, only dictionaries can be merged together.
            """,
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
    args.add_all([keyValue for keyValue in ctx.attr.set_flag[BuildSettingInfo].value if keyValue] if ctx.attr.set_flag else [], before_each = "--override-set")

    [file, runfiles, data] = _from_attr_to_config_info(ctx, ctx.attr.base)
    args.add("--base", file)

    # Build the default configuration.
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
        "py": ctx.outputs.output_py,
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
        "output_py": attr.output(
            doc = "Create a python configuration.",
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

def _bzd_config_macro_impl(name, visibility, output_json, output_yaml, output_py, **kwargs):
    _bzd_config(
        name = "{}.base".format(name),
        **kwargs
    )
    _bzd_config_convert(
        name = name,
        config = "{}.base".format(name),
        output_json = output_json if output_json else "{}.json".format(name),
        output_yaml = output_yaml if output_yaml else "{}.yaml".format(name),
        output_py = output_py if output_py else "{}.py".format(name),
        visibility = visibility,
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
        "output_py": attr.string(
            doc = "Name of the generated python configuration.",
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

def _bzd_config_default_macro_impl(name, visibility, output_json, output_yaml, output_py, **kwargs):
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
        output_py = output_py if output_py else "{}.py".format(name),
        visibility = visibility,
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
        "output_py": attr.string(
            doc = "Name of the generated python configuration.",
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
