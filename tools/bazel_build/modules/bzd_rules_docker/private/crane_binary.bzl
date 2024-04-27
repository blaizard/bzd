"""Create a wrapper for crane binary."""

def _bzd_crane_binary_impl(ctx):
    toolchain = ctx.toolchains["@rules_oci//oci:registry_toolchain_type"]
    executable = ctx.actions.declare_file(ctx.label.name)
    ctx.actions.symlink(
        output = executable,
        target_file = toolchain.registry_info.registry,
        is_executable = True,
    )

    return DefaultInfo(
        executable = executable,
        runfiles = toolchain.default.default_runfiles,
    )

bzd_crane_binary = rule(
    doc = "Wrapper for the crane binary.",
    implementation = _bzd_crane_binary_impl,
    provides = [DefaultInfo],
    toolchains = [
        "@rules_oci//oci:registry_toolchain_type",
    ],
    executable = True,
)
