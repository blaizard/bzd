"""Configuration rules."""

load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")

def _bzd_config_flag_impl(ctx):
    return BuildSettingInfo(value = ctx.build_setting_value)

_bzd_config_flag = rule(
    implementation = _bzd_config_flag_impl,
    build_setting = config.string(flag = True, allow_multiple = True),
)

def _bzd_config_impl(ctx):
    key_values = [keyValue for keyValue in ctx.attr.set_flag[BuildSettingInfo].value if keyValue]
    files = ctx.files.file_flag

    input_files = []
    input_files += ctx.files.srcs

    # If there are inline values, create an additional input file.
    if ctx.attr.values:
        values_file = ctx.actions.declare_file(ctx.label.name + ".values.json")
        ctx.actions.write(
            output = values_file,
            content = json.encode(ctx.attr.values),
        )
        input_files.append(values_file)

    # Build the default configuration.
    ctx.actions.run(
        inputs = input_files + files,
        outputs = [ctx.outputs.output_json],
        progress_message = "Generating default configuration for {}...".format(ctx.label),
        arguments = [
                        "--fail-on-conflict",
                        "--output",
                        ctx.outputs.output_json.path,
                    ] +
                    ["--set=" + key_value for key_value in key_values] +
                    ["--file=" + f.path for f in files] +
                    [f.path for f in input_files],
        executable = ctx.executable._config_merge,
    )

    return [DefaultInfo(files = depset([ctx.outputs.output_json]))]

_bzd_config = rule(
    implementation = _bzd_config_impl,
    attrs = {
        "file_flag": attr.label(
            doc = "Build settings to modify the configuration using files.",
            allow_files = True,
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
            default = Label("@bzd_lib//config:merge"),
            cfg = "exec",
            executable = True,
        ),
    },
)

def bzd_config(name, **kwargs):
    """Macro to create a configuration."""

    _bzd_config_flag(
        name = name + ".set",
        build_setting_default = "",
    )

    native.label_flag(
        name = name + ".file",
        build_setting_default = "@bzd_lib//config:empty",
    )

    _bzd_config(
        name = name,
        set_flag = ":{}.set".format(name),
        file_flag = ":{}.file".format(name),
        output_json = "{}.json".format(name),
        **kwargs
    )
