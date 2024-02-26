"""Static file generator from a user provided script."""

load("@bzd_rules_nodejs//nodejs:private/nodejs_install.bzl", "BzdNodeJsInstallInfo")

def _update_argument(arg):
    if arg.startswith("{config:"):
        return "\"$(config {})\"".format(arg[8:-1])
    return arg.replace(" ", "\\ ")

def _bzd_nodejs_static_impl(ctx):
    wrapper = ctx.actions.declare_file("{}.wrapper".format(ctx.label.name))
    output = ctx.actions.declare_file(ctx.label.name)

    # Convert arguments into a command string.
    args = []
    for arg in ctx.attr.args:
        arg = _update_argument(arg).format(
            api = ctx.attr.install[BzdNodeJsInstallInfo].api.path,
            output = output.path,
        )
        args.append(arg)

    ctx.actions.write(
        output = wrapper,
        is_executable = True,
        content = """#!/usr/bin/env bash
set -o errexit
config () {{
    {reader} {config} "$1"
}}
{script} {args}
""".format(
            script = ctx.executable.script.path,
            reader = ctx.executable._config_reader.path,
            config = ctx.file.config.path,
            args = " ".join(args),
        ),
    )

    ctx.actions.run(
        outputs = [output],
        executable = wrapper,
        tools = [
            ctx.executable.script,
            ctx.executable._config_reader,
            ctx.attr.install[BzdNodeJsInstallInfo].api,
            ctx.file.config,
        ],
    )

    return DefaultInfo(files = depset([output]))

bzd_nodejs_static = rule(
    doc = "NodeJs output static file generator.",
    implementation = _bzd_nodejs_static_impl,
    attrs = {
        "args": attr.string_list(
            doc = "Arguments to be passsed to the script, accepts the followng subsitution: '{config:<key>}', '{output}', '{api}'",
        ),
        "config": attr.label(
            allow_single_file = True,
            doc = "The configuration file to be used.",
        ),
        "install": attr.label(
            mandatory = True,
            providers = [BzdNodeJsInstallInfo],
            doc = "The nodejs installation target.",
        ),
        "script": attr.label(
            executable = True,
            cfg = "exec",
            mandatory = True,
            doc = "A script to run to generate the output.",
        ),
        "_config_reader": attr.label(
            executable = True,
            cfg = "exec",
            default = Label("@bzd_lib//config:reader"),
        ),
    },
)
