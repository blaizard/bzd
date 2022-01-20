# Custom provider for a manifest
BzdManifestInfo = provider(fields = ["manifest", "artifacts"])

def _bzd_manifest_impl(ctx):
    return [DefaultInfo(), CcInfo(), BzdManifestInfo(
        manifest = depset(transitive = [f.files for f in ctx.attr.manifest]),
        artifacts = depset([(f.files.to_list()[0], key) for f, key in ctx.attr.artifacts.items()]),
    )]

"""
Rule to declare a bzd manifests.
"""

bzd_manifest = rule(
    implementation = _bzd_manifest_impl,
    attrs = {
        "manifest": attr.label_list(
            allow_files = True,
            mandatory = True,
            doc = "Input manifest files.",
        ),
        "artifacts": attr.label_keyed_string_dict(
            allow_files = True,
            allow_empty = True,
            doc = "Artifacts to be added to the app.",
        ),
        "deps": attr.label_list(
            allow_files = True,
        ),
    },
)

def _bzd_manifest_aspect_impl(target, ctx):
    if ctx.rule.kind == "bzd_manifest":
        return []
    return [
        BzdManifestInfo(
            manifest = depset(transitive = [dep[BzdManifestInfo].manifest for dep in ctx.rule.attr.deps]),
            artifacts = depset(transitive = [dep[BzdManifestInfo].artifacts for dep in ctx.rule.attr.deps]),
        ),
    ]

"""
Aspects to gather data from bzd depedencies.
"""
bzd_manifest_aspect = aspect(
    implementation = _bzd_manifest_aspect_impl,
    attr_aspects = ["deps"],
)

# ---- Manifest build

def _bzd_manifest_build_impl(ctx):
    manifests = depset(transitive = [dep[BzdManifestInfo].manifest for dep in ctx.attr.deps]).to_list()
    artifacts = depset(transitive = [dep[BzdManifestInfo].artifacts for dep in ctx.attr.deps]).to_list()

    # Build the argument list
    args = ctx.actions.args()
    args.add("--format")
    args.add(ctx.attr.format)
    args.add("--output")
    args.add(ctx.outputs.output.path)
    outputs = [ctx.outputs.output]
    inputs = list(manifests)

    if ctx.outputs.output_manifest:
        args.add("--manifest")
        args.add(ctx.outputs.output_manifest.path)
        outputs.append(ctx.outputs.output_manifest)

    # Add artifacts
    for artifact in artifacts:
        args.add("--artifact")
        args.add("{}:{}".format(artifact[1], artifact[0].path))
        inputs.append(artifact[0])

    # Add the manifest list
    [args.add(f.path) for f in manifests]

    # Generate the files
    ctx.actions.run(
        outputs = outputs,
        inputs = inputs,
        arguments = [args],
        progress_message = "Generating bzd files...",
        executable = ctx.executable._generator,
    )

    return [
        OutputGroupInfo(all_files = depset(outputs)),
        CcInfo(),
    ]

"""
Rule to define the bzd file generator.
"""
bzd_manifest_build = rule(
    implementation = _bzd_manifest_build_impl,
    output_to_genfiles = True,
    attrs = {
        "deps": attr.label_list(
            aspects = [bzd_manifest_aspect],
            doc = "Dependencies used to build this target.",
        ),
        "format": attr.string(
            mandatory = True,
            doc = "Format to be used for the file generation.",
        ),
        "output": attr.output(
            mandatory = True,
            doc = "Define the output file path.",
        ),
        "output_manifest": attr.output(
            doc = "Define the merged manifest output for this file.",
        ),
        "_generator": attr.label(
            default = Label("//tools/bzd/generator"),
            cfg = "exec",
            executable = True,
        ),
    },
)
