"""Access the rustfmt binary."""

def _rustfmt_impl(ctx):
    info = ctx.toolchains["@rules_rust//rust/rustfmt:toolchain_type"]

    # Create an alias.
    executable = ctx.actions.declare_file(ctx.label.name)
    ctx.actions.symlink(
        output = executable,
        target_file = info.rustfmt,
    )

    return [DefaultInfo(
        executable = executable,
        runfiles = ctx.runfiles(transitive_files = info.all_files),
    )]

rustfmt_binary = rule(
    implementation = _rustfmt_impl,
    attrs = {},
    toolchains = ["@rules_rust//rust/rustfmt:toolchain_type"],
    executable = True,
)
