"""
C++ specific provider
"""

def _bzd_manifest_impl_cc(ctx, output):
    compilation_context = cc_common.create_compilation_context(
        headers = depset([output]),
        includes = depset([ctx.bin_dir.path]),
    )

    return [CcInfo(compilation_context = compilation_context)]

def _bzd_manifest_impl(ctx):
    formats = {
        "cc": {
            "display": "C++",
            "extension": ".h",
        },
    }

    for fmt, data in formats.items():
        # Generate the output
        data["output"] = ctx.actions.declare_file("{}{}".format(ctx.label.name, data["extension"]))
        ctx.actions.run(
            inputs = ctx.files.srcs,
            outputs = [data["output"]],
            progress_message = "Generating {} build files from manifest {}".format(data["display"], ctx.label),
            arguments = ["--format", fmt, "--output", data["output"].path] + [f.path for f in ctx.files.srcs],
            executable = ctx.attr._bdl.files_to_run,
        )

    # Generate the various providers
    providerCc = _bzd_manifest_impl_cc(ctx = ctx, output = formats["cc"]["output"])

    return providerCc

"""
Bzd description language rule_bzd_manifest_impl_cc
"""
bzd_manifest = rule(
    implementation = _bzd_manifest_impl,
    doc = """Bzd Description Language generator rule.
    It generates language provider from a .bdl file.
    The files are generated at the same path of the target, with the name of the target appended with a language specific file extension.
    """,
    attrs = {
        "srcs": attr.label_list(
            mandatory = True,
            allow_files = [".bdl"],
            doc = "List of Bzd Description Language (bdl) files to be included.",
        ),
        "_bdl": attr.label(
            default = Label("//tools/bdl"),
            cfg = "host",
            executable = True,
        ),
    },
)
