"""Static file generator from a user provided script."""

load("@bzd_rules_nodejs//nodejs:private/nodejs_install.bzl", "BzdNodeJsInstallInfo")

def _update_argument(arg, reader, config, **kwargs):
    if arg.startswith("{config:"):
        return "{reader} {config} {arg}".format(
            reader = reader,
            config = config,
            arg = arg[8:-1],
        )
    return "echo " + arg.format(**kwargs)

def _bzd_nodejs_static_impl(ctx):
    output = ctx.actions.declare_file(ctx.label.name)

    # Convert arguments into a command string.
    params = []
    for arg in ctx.attr.args:
        arg = _update_argument(
            arg,
            reader = ctx.executable._config_reader.path,
            config = ctx.file.config.path,
            api = ctx.attr.install[BzdNodeJsInstallInfo].api.path,
            output = output.path,
        )
        params.append(arg)

    # Create the param file.
    param_file = ctx.actions.declare_file(ctx.label.name + ".param")
    ctx.actions.run_shell(
        outputs = [param_file],
        tools = [
            ctx.executable._config_reader,
            ctx.file.config,
        ],
        command = "set -o errexit\n" + "\n".join([arg + " >> " + param_file.path for arg in params]),
    )

    # Read and call the script with the arguments unpacked.
    ctx.actions.run_shell(
        outputs = [output],
        command = """
mapfile -t < {params}
{script} "${{MAPFILE[@]}}"
""".format(
            script = ctx.executable.script.path,
            params = param_file.path,
        ),
        tools = [
            ctx.executable.script,
            ctx.attr.install[BzdNodeJsInstallInfo].api,
            param_file,
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
