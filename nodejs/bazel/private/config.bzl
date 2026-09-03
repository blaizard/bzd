"""Generate a configuration file with nodejs bindings."""

load("@bzd_lib//config:defs.bzl", "ConfigInfo")
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

def _bzd_nodejs_config_generator_impl(ctx):
    config_info = ctx.attr.config[ConfigInfo]
    output = ctx.actions.declare_file(ctx.attr.module_name + ".ts")
    ctx.actions.run(
        inputs = [config_info.internal],
        outputs = [output],
        arguments = [
            "--output",
            output.path,
            "--internal",
            config_info.internal.path,
            "--expose",
            json.encode(ctx.attr.expose),
        ],
        progress_message = "Converting configuration to TypeScript for {}...".format(ctx.label),
        executable = ctx.executable._config_generator,
    )

    return [
        DefaultInfo(runfiles = config_info.runfiles, files = depset([output])),
    ]

_bzd_nodejs_config_generator = rule(
    implementation = _bzd_nodejs_config_generator_impl,
    doc = "Generate a configuration file with nodejs bindings.",
    attrs = {
        "config": attr.label(
            providers = [ConfigInfo],
            doc = "The configuration to be used.",
            mandatory = True,
        ),
        "expose": attr.string_dict(
            doc = "The configuration keys to expose.",
            mandatory = True,
        ),
        "module_name": attr.string(
            doc = "The name of the module to be generated.",
            mandatory = True,
        ),
        "_config_generator": attr.label(
            doc = "The config generator binary.",
            default = Label("//nodejs/bazel/private:config_generator"),
            cfg = "exec",
            executable = True,
        ),
    },
)

def _bzd_nodejs_config_impl(name, visibility, config, expose, **kwargs):
    _bzd_nodejs_config_generator(
        name = "{}.generate".format(name),
        config = config,
        expose = expose,
        module_name = name,
        tags = ["manual"],
    )

    bzd_nodejs_library(
        name = name,
        srcs = ["{}.generate".format(name)],
        data = [":{}.generate".format(name)],
        deps = [
            Label("//nodejs/utils:runfiles"),
            Label("//nodejs/utils:secret"),
        ],
        visibility = visibility,
        **kwargs
    )

bzd_nodejs_config = macro(
    implementation = _bzd_nodejs_config_impl,
    inherit_attrs = "common",
    doc = "Generate a configuration file with nodejs bindings.",
    attrs = {
        "config": attr.label(
            providers = [ConfigInfo],
            doc = "The configuration to be used.",
            mandatory = True,
        ),
        "expose": attr.string_dict(
            doc = "The configuration keys to expose.",
            mandatory = True,
        ),
    },
)
