"""Configuration rules."""

load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")
load("//config:private/common.bzl", "ConfigInfo")

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
    args.add("--fail-on-conflict")
    args.add("--output", ctx.outputs.output_json)
    args.add_all([keyValue for keyValue in ctx.attr.set_flag[BuildSettingInfo].value if keyValue] if ctx.attr.set_flag else [], before_each = "--set")

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

    # Handle override with files.
    if ctx.attr.file_flag and (ctx.attr.file_flag.label != Label("//config:empty")):
        override_files = []
        if ConfigInfo in ctx.attr.file_flag:
            override_files.append(ctx.attr.file_flag[ConfigInfo].json)
            runfiles = runfiles.merge(ctx.attr.file_flag[ConfigInfo].runfiles)
            data = depset(transitive = [data, ctx.attr.file_flag[ConfigInfo].data])
        elif ctx.files.file_flag:
            override_files += ctx.files.file_flag
        else:
            fail("Invalid config override target type: {} {}".format(ctx.attr.file_flag.label))
        args.add_all(override_files, before_each = "--file")
        input_files = depset(override_files, transitive = [input_files])

    # Build the default configuration.
    ctx.actions.run(
        inputs = input_files,
        outputs = [ctx.outputs.output_json],
        progress_message = "Generating default configuration for {}...".format(ctx.label),
        arguments = [args],
        executable = ctx.executable._config_merge,
    )

    # Create the additional outputs
    outputs = {
        "py": ctx.outputs.output_py,
        "yaml": ctx.outputs.output_yaml,
    }
    for format, file in outputs.items():
        ctx.actions.run(
            inputs = [ctx.outputs.output_json],
            outputs = [file],
            arguments = [
                "--output",
                file.path,
                "--json",
                ctx.outputs.output_json.path,
                "--format",
                format,
            ],
            executable = ctx.executable._config_convert,
        )

    return [
        ConfigInfo(json = ctx.outputs.output_json, runfiles = runfiles, data = data),
        DefaultInfo(runfiles = runfiles, files = data),
    ]

_bzd_config = rule(
    implementation = _bzd_config_impl,
    attrs = {
        "data": attr.label_list(
            doc = "Dependencies for this rule, will be added in the runfiles and used for target expansion.",
            allow_files = True,
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
        "output_py": attr.output(
            doc = "Create a python configuration.",
        ),
        "output_yaml": attr.output(
            doc = "Create a yaml configuration.",
        ),
        "set_flag": attr.label(
            doc = "Build settings to modify the configuration using key/value pair.",
            providers = [BuildSettingInfo],
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
        "_config_convert": attr.label(
            default = Label("//config:convert"),
            cfg = "exec",
            executable = True,
        ),
        "_config_merge": attr.label(
            default = Label("//config:merge"),
            cfg = "exec",
            executable = True,
        ),
    },
    provides = [DefaultInfo, ConfigInfo],
)

def _bzd_config_macro_impl(name, visibility, output_json, output_yaml, output_py, **kwargs):
    _bzd_config(
        name = name,
        visibility = visibility,
        output_json = output_json if output_json else "{}.json".format(name),
        output_yaml = output_yaml if output_yaml else "{}.yaml".format(name),
        output_py = output_py if output_py else "{}.py".format(name),
        **kwargs
    )

bzd_config = macro(
    doc = "Create a configuration that cannot be overwritten.",
    implementation = _bzd_config_macro_impl,
    inherit_attrs = _bzd_config,
    attrs = {
        "file_flag": None,
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
        "set_flag": None,
    },
)

def _bzd_config_default_macro_impl(name, visibility, output_json, output_yaml, output_py, **kwargs):
    _bzd_config_flag(
        name = "{}.set".format(name),
        build_setting_default = "",
    )

    native.label_flag(
        name = "{}.file".format(name),
        build_setting_default = Label("//config:empty"),
    )

    _bzd_config(
        name = name,
        visibility = visibility,
        set_flag = ":{}.set".format(name),
        file_flag = ":{}.file".format(name),
        output_json = output_json if output_json else "{}.json".format(name),
        output_yaml = output_yaml if output_yaml else "{}.yaml".format(name),
        output_py = output_py if output_py else "{}.py".format(name),
        **kwargs
    )

bzd_config_default = macro(
    doc = "Create a default configuration that can be overwritten.",
    implementation = _bzd_config_default_macro_impl,
    inherit_attrs = _bzd_config,
    attrs = {
        "file_flag": None,
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
        "set_flag": None,
    },
)
