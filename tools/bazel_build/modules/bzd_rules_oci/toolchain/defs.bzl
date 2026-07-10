"""Toolchain definitions for OCI images."""

def _oci_toolchain_impl(ctx):
    return platform_common.ToolchainInfo(
        oci_runner = ctx.attr.oci_runner,
    )

oci_toolchain = rule(
    implementation = _oci_toolchain_impl,
    attrs = {
        "oci_runner": attr.label(
            mandatory = True,
            executable = True,
            cfg = "target",
        ),
    },
)
