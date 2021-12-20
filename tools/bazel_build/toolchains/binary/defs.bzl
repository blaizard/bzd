BinaryInfo = provider(
    doc = "Binary toolchain, to prepare and execute a binary for a given execution platform.",
    fields = ["prepare", "metadatas", "executors"],
)

def _binary_toolchain_impl(ctx):
    toolchain_info = platform_common.ToolchainInfo(
        info = BinaryInfo(
            prepare = ctx.attr.prepare,
            metadatas = ctx.attr.metadatas,
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
        "prepare": attr.label(
            executable = True,
            cfg = "host",
        ),
        "metadatas": attr.label_list(
            allow_files = True,
        ),
        "executors": attr.label_keyed_string_dict(
            cfg = "host",
        ),
    },
)
