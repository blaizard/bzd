"""Defines a binary toolchain."""

BinaryInfo = provider(
    doc = "Binary toolchain, to build and execute an application for a given execution platform.",
    fields = ["build", "metadata", "executors"],
)

def _binary_toolchain_impl(ctx):
    toolchain_info = platform_common.ToolchainInfo(
        info = BinaryInfo(
            build = ctx.attr.build,
            metadata = ctx.attr.metadata,
            executors = ctx.attr.executors,
        ),
    )
    return [toolchain_info]

binary_toolchain = rule(
    implementation = _binary_toolchain_impl,
    attrs = {
        "build": attr.label_list(
            providers = [DefaultInfo],
            cfg = "exec",
        ),
        "executors": attr.label_keyed_string_dict(
            doc = "Executor binaries that run on the host system and execute the target.",
            providers = [DefaultInfo],
            cfg = "exec",
        ),
        "metadata": attr.label_list(
            allow_files = True,
        ),
    },
)
