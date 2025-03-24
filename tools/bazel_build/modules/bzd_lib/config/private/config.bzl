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
    key_values = [keyValue for keyValue in ctx.attr.set_flag[BuildSettingInfo].value if keyValue] if ctx.attr.set_flag else []

    input_files = depset(ctx.files.srcs)
    override_files = depset()
    workspace_status_files = []
    runfiles = ctx.runfiles().merge_all([target.default_runfiles for target in ctx.attr.data])
    data = depset(ctx.files.data)

    # If there are inline values, create an additional input file.
    if ctx.attr.values:
        values_file = ctx.actions.declare_file(ctx.label.name + ".values.json")
        expanded_values = {}
        for key, value in ctx.attr.values.items():
            current = expanded_values

            # Expands the '.' to a nested dictionary.
            parts = key.split(".")
            if len(parts) > 1:
                for part in parts[:-1]:
                    current.setdefault(part, {})
                    current = current[part]
            current[parts[-1]] = ctx.expand_location(value, targets = ctx.attr.data)
        ctx.actions.write(
            output = values_file,
            content = json.encode(expanded_values),
        )
        input_files = depset([values_file], transitive = [input_files])

    if ctx.attr.include_workspace_status:
        workspace_status_files.append(ctx.info_file)
        workspace_status_files.append(ctx.version_file)

    if ctx.attr.file_flag:
        if ctx.attr.file_flag.label == Label("//config:empty"):
            pass
        elif ConfigInfo in ctx.attr.file_flag:
            override_files = depset([ctx.attr.file_flag[ConfigInfo].json], transitive = [override_files])
            runfiles = runfiles.merge(ctx.attr.file_flag[ConfigInfo].runfiles)
            data = depset(transitive = [data, ctx.attr.file_flag[ConfigInfo].data])
        elif ctx.files.file_flag:
            override_files = depset(ctx.files.file_flag, transitive = [override_files])
        else:
            fail("Invalid config override target type: {} {}".format(ctx.attr.file_flag.label))

    # Build the default configuration.
    ctx.actions.run(
        inputs = depset(workspace_status_files, transitive = [input_files, override_files]),
        outputs = [ctx.outputs.output_json],
        progress_message = "Generating default configuration for {}...".format(ctx.label),
        arguments = [
                        "--fail-on-conflict",
                        "--output",
                        ctx.outputs.output_json.path,
                    ] +
                    ["--set=" + key_value for key_value in key_values] +
                    ["--file=" + f.path for f in override_files.to_list()] +
                    ["--workspace-status-file=" + f.path for f in workspace_status_files] +
                    ["--workspace-status-key=" + key for key in ctx.attr.include_workspace_status] +
                    [f.path for f in input_files.to_list()],
        executable = ctx.executable._config_merge,
    )

    # Create a python output
    ctx.actions.run(
        inputs = [ctx.outputs.output_json, ctx.file._template_py],
        outputs = [ctx.outputs.output_py],
        arguments = [
            "--output",
            ctx.outputs.output_py.path,
            "--json",
            ctx.outputs.output_json.path,
            ctx.file._template_py.path,
        ],
        executable = ctx.executable._config_template,
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
    provides = [DefaultInfo, ConfigInfo],
)

def _bzd_config_macro_impl(name, visibility, output_json, output_py, **kwargs):
    _bzd_config(
        name = name,
        visibility = visibility,
        output_json = output_json if output_json else "{}.json".format(name),
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
        "set_flag": None,
    },
)

def _bzd_config_default_macro_impl(name, visibility, output_json, output_py, **kwargs):
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
        "set_flag": None,
    },
)
