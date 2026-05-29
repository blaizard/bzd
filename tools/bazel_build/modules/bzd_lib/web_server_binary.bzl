"""Binary rule to run a webserver."""

load("//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")

def _bzd_bzd_web_server_binary_impl(ctx):
    return [
        sh_binary_wrapper_impl(
            ctx = ctx,
            locations = {
                ctx.attr._web_server: "binary",
                ctx.file.static: "static",
            },
            output = ctx.outputs.executable,
            command = "{binary} {static} $@",
        ),
    ]

bzd_web_server_binary = rule(
    doc = "Rule to run a web server to serve static content.",
    implementation = _bzd_bzd_web_server_binary_impl,
    attrs = {
        "static": attr.label(
            allow_single_file = True,
            doc = "Static content to server. It should be a single directory.",
        ),
        "_web_server": attr.label(
            executable = True,
            cfg = "target",
            default = Label("//:web_server"),
        ),
    },
    executable = True,
)
