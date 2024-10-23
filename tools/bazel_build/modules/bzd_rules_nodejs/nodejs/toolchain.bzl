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

def _nodejs_manager_binary_impl(ctx):
    manager = ctx.toolchains["//nodejs:toolchain_type"].executable.manager

    ctx.actions.symlink(
        target_file = manager.files_to_run.executable,
        is_executable = True,
        output = ctx.outputs.executable,
    )

    return DefaultInfo(
        executable = ctx.outputs.executable,
        runfiles = manager.default_runfiles,
    )

nodejs_manager_binary = rule(
    doc = "Expose the manager binary from the toolchain.",
    implementation = _nodejs_manager_binary_impl,
    attrs = {},
    toolchains = ["//nodejs:toolchain_type"],
    executable = True,
)
