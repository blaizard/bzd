NodeJsToolchainProvider = provider(
    fields = ["manager", "node"],
)

def _nodejs_toolchain_impl(ctx):
    toolchain_info = platform_common.ToolchainInfo(
        executable = NodeJsToolchainProvider(
            manager = ctx.attr.manager,
            node = ctx.attr.node,
        ),
    )
    return [toolchain_info]

"""
Defines a NodeJs toolchain.
"""
nodejs_toolchain = rule(
    implementation = _nodejs_toolchain_impl,
    attrs = {
        "manager": attr.label(
            mandatory = True,
            executable = True,
            cfg = "host",
        ),
        "node": attr.label(
            mandatory = True,
            executable = True,
            cfg = "host",
        ),
    },
)
