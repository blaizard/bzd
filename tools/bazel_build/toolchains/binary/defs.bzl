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

"""
Defines a binary toolchain.
"""
binary_toolchain = rule(
    implementation = _binary_toolchain_impl,
    attrs = {
        "build": attr.label_list(
            cfg = "host",
        ),
        "metadata": attr.label_list(
            allow_files = True,
        ),
        "executors": attr.label_keyed_string_dict(
            cfg = "host",
        ),
    },
)
