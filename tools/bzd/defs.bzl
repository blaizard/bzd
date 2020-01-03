load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

# Custom provider for a manifest
BzdManifestInfo = provider(fields = ["manifest"])

def _bzd_manifest_impl(ctx):
    return [DefaultInfo(), CcInfo(), BzdManifestInfo(manifest = depset(transitive = [f.files for f in ctx.attr.manifest]))]

"""
Rule for bzd manifests.
"""
bzd_manifest = rule(
    implementation = _bzd_manifest_impl,
    attrs = {
        "manifest": attr.label_list(
            allow_files = True,
            mandatory = True,
            doc = "Input manifest files.",
        ),
        "deps": attr.label_list(
            allow_files = True,
        ),
    },
)

def _bzd_deps_aspect_impl(target, ctx):
    if ctx.rule.kind == "bzd_manifest":
        return []
    return [
        BzdManifestInfo(manifest = depset(transitive = [dep[BzdManifestInfo].manifest for dep in ctx.rule.attr.deps])),
    ]

"""
Aspects to gather data from bzd depedencies.
"""
bzd_deps_aspect = aspect(
    implementation = _bzd_deps_aspect_impl,
    attr_aspects = ["deps"],
)

def _bzd_generate_rule_impl(ctx):
    manifests = depset(transitive = [dep[BzdManifestInfo].manifest for dep in ctx.attr.deps]).to_list()

    # Build the argument list
    args = ctx.actions.args()
    args.add("--format")
    args.add(ctx.attr.format)
    args.add("--output")
    args.add(ctx.outputs.output.path)
    outputs = [ctx.outputs.output]

    if ctx.outputs.output_manifest:
        args.add("--manifest")
        args.add(ctx.outputs.output_manifest.path)
        outputs += [ctx.outputs.output_manifest]

    # Add the manifest list
    [args.add(f.path) for f in manifests]

    # Generate the files
    ctx.actions.run(
        outputs = outputs,
        inputs = manifests,
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
bzd_generate_rule = rule(
    implementation = _bzd_generate_rule_impl,
    output_to_genfiles = True,
    attrs = {
        "deps": attr.label_list(
            aspects = [bzd_deps_aspect],
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
            cfg = "host",
            executable = True,
        ),
    },
)

def _bzd_cc_macro_impl(is_test, name, deps, **kwargs):
    # Generates the auto-generated files to be compiled with the project
    output_cc = ".bzd/{}.cpp".format(name)
    output_manifest = ".bzd/{}.manifest".format(name)
    manifest_rule_name = "{}_manifest".format(name)
    bzd_generate_rule(
        name = manifest_rule_name,
        deps = deps,
        format = "cpp",
        output = output_cc,
        output_manifest = output_manifest,
    )

    # Generates a library from the auto-generated files
    library_rule_name = "{}_library".format(name)
    cc_library(
        name = library_rule_name,
        srcs = [output_cc],
        deps = [manifest_rule_name] + deps,
        alwayslink = True,
    )

    # Call the binary/test rule
    rule_to_use = cc_test if is_test else cc_binary
    rule_to_use(
        name = name,
        deps = deps + [library_rule_name],
        **kwargs
    )

"""
Rule to define a bzd binary.
"""

def bzd_cc_binary(name, deps, **kwargs):
    return _bzd_cc_macro_impl(False, name, deps, **kwargs)

"""
Rule to define a bzd test.
"""

def bzd_cc_test(name, deps, **kwargs):
    return _bzd_cc_macro_impl(True, name, deps, **kwargs)
