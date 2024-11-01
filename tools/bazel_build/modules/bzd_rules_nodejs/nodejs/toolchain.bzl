"""NodeJs toolchain."""

NodeJsToolchainInfo = provider(
    "Provide NodeJs toolchain information",
    fields = ["manager", "node"],
)

def _nodejs_toolchain_impl(ctx):
    toolchain_info = platform_common.ToolchainInfo(
        executable = NodeJsToolchainInfo(
            manager = ctx.attr.manager,
            node = ctx.attr.node,
        ),
    )
    return [toolchain_info]

nodejs_toolchain = rule(
    doc = "Defines a NodeJs toolchain.",
    implementation = _nodejs_toolchain_impl,
    attrs = {
        "manager": attr.label(
            mandatory = True,
            executable = True,
            cfg = "exec",
        ),
        "node": attr.label(
            mandatory = True,
            executable = True,
            cfg = "exec",
        ),
    },
)
