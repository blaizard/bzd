"""Static file generator from a user provided script."""

load("@bzd_rules_nodejs//nodejs:private/nodejs_install.bzl", "BzdNodeJsInstallInfo")

def _bzd_nodejs_generator_impl(ctx):
    wrapper = ctx.actions.declare_file("{}.wrapper".format(ctx.label.name))
    output = ctx.actions.declare_file(ctx.label.name)

    ctx.actions.write(
        output = wrapper,
        is_executable = True,
        content = """#!/usr/bin/env bash
set -e
config () {{
    {reader} {config} "$1"
}}
""".format(
            reader = ctx.executable._config_reader.path,
            config = ctx.file.config.path,
        ) + ctx.attr.command.format(
            script = ctx.executable.script.path,
            api = ctx.attr.install[BzdNodeJsInstallInfo].api.path,
            output = output.path,
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

bzd_nodejs_generator = rule(
    doc = "NodeJs output generator.",
    implementation = _bzd_nodejs_generator_impl,
    attrs = {
        "command": attr.string(
            doc = "Command to run, accepts the followng subsitution: '$(config <key>)', '{script}', '{output}', '{api}'",
            default = "{script} -o {output}",
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
