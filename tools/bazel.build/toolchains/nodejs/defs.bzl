NodeJsToolchainProvider = provider(
    fields = ["binary"],
)

def _nodejs_toolchain_impl(ctx):
    toolchain_info = platform_common.ToolchainInfo(
        manager = NodeJsToolchainProvider(
            binary = ctx.attr.binary,
        ),
    )
    return [toolchain_info]

"""
Defines a NodeJs toolchain.
"""
nodejs_toolchain = rule(
    implementation = _nodejs_toolchain_impl,
    attrs = {
        "binary": attr.label(
            mandatory = True,
            executable = True,
            cfg = "host",
        ),
    },
)
