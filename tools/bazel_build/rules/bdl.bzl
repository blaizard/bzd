"""
C++ specific provider
"""

def _bzd_manifest_impl_cc(ctx, output, includeDir):
    compilation_context = cc_common.create_compilation_context(
        headers = depset([output]),
        includes = depset([includeDir]),
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
        data["output"] = ctx.actions.declare_file("bzd-auto/{}/{}{}".format(ctx.label.package, ctx.label.name, data["extension"]))
        ctx.actions.run(
            inputs = ctx.files.srcs,
            outputs = [data["output"]],
            progress_message = "Generating {} build files from manifest {}".format(data["display"], ctx.label),
            arguments = ["--format", fmt, "--output", data["output"].path] + [f.path for f in ctx.files.srcs],
            executable = ctx.attr._bzd.files_to_run,
        )

    # Identify the include path
    sampleOutput = formats["cc"]["output"]
    index = sampleOutput.path.find("bzd-auto")
    if index == -1:
        fail("Cannot find 'bzd-auto' within newly create artifact path: {}".format(sampleOutput))
    includeDir = sampleOutput.path[0:index]

    # Generate the various providers
    providerCc = _bzd_manifest_impl_cc(ctx = ctx, output = formats["cc"]["output"], includeDir = includeDir)

    return providerCc

"""
Bzd description language rule_bzd_manifest_impl_cc
"""
bzd_manifest = rule(
    implementation = _bzd_manifest_impl,
    attrs = {
        "srcs": attr.label_list(
            mandatory = True,
            allow_files = True,
            doc = "List of .bdl files to be included.",
        ),
        "_bzd": attr.label(
            default = Label("//tools/bzd2"),
            cfg = "host",
            executable = True,
        ),
    },
)
