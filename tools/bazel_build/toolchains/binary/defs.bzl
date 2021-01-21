BinaryInfo = provider(
    doc = "Binary toolchain, to prepare and execute a binary for a given execution platform.",
    fields = ["prepare", "metadatas", "execute"],
)

def _binary_toolchain_impl(ctx):
    toolchain_info = platform_common.ToolchainInfo(
        info = BinaryInfo(
            prepare = ctx.attr.prepare,
            metadatas = ctx.attr.metadatas,
            execute = ctx.attr.execute,
        ),
    )
    return [toolchain_info]

"""
Defines a binary toolchain.
"""
binary_toolchain = rule(
    implementation = _binary_toolchain_impl,
    attrs = {
        "prepare": attr.label(
            executable = True,
            cfg = "host",
        ),
        "metadatas": attr.label_list(
            allow_files = True,
        ),
        "execute": attr.label(
            executable = True,
            cfg = "host",
        ),
    },
)
