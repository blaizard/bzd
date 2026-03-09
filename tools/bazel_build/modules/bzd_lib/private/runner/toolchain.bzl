"""Runner toolchain definition."""

def _bzd_runner_toolchain_impl(ctx):
    toolchain_info = platform_common.ToolchainInfo(
        runner = ctx.attr.runner,
    )
    return [toolchain_info]

bzd_runner_toolchain = rule(
    implementation = _bzd_runner_toolchain_impl,
    attrs = {
        "runner": attr.label(
            executable = True,
            cfg = "exec",
            mandatory = False,
        ),
    },
    provides = [platform_common.ToolchainInfo],
)

def _bzd_make_runner_toolchain_impl(name, visibility, runner, **kwargs):
    bzd_runner_toolchain(
        name = "{}.toolchain".format(name),
        runner = runner,
    )
    native.toolchain(
        name = name,
        toolchain_type = Label("//private/runner:toolchain_type"),
        toolchain = ":{}.toolchain".format(name),
        visibility = visibility,
        **kwargs
    )

bzd_make_runner_toolchain = macro(
    inherit_attrs = native.toolchain,
    implementation = _bzd_make_runner_toolchain_impl,
    attrs = {
        "runner": attr.label(
            executable = True,
            cfg = "exec",
            mandatory = False,
            doc = "The runner to use for this toolchain. If not set, the binary will be run directly.",
        ),
        "toolchain": None,
        "toolchain_type": None,
    },
)
