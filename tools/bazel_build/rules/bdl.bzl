load("@bazel_skylib//lib:new_sets.bzl", "sets")

BdlProvider = provider(fields = ["outputs"])

def _bzd_manifest_impl_cc(ctx, inputs):
    """
    C++ specific provider
    """
    compilation_context = cc_common.create_compilation_context(
        headers = depset(inputs),
        includes = depset([ctx.bin_dir.path]),
    )

    cc_info_providers = sets.make()
    sets.insert(cc_info_providers, CcInfo(compilation_context = compilation_context))

    # Merge providers from deps rules
    for dep in ctx.attr._deps_cc:
        sets.insert(cc_info_providers, dep[CcInfo])
    cc_info_providers = cc_common.merge_cc_infos(cc_infos = sets.to_list(cc_info_providers))

    return cc_info_providers

def _bzd_manifest_impl(ctx):
    formats = {
        "cc": {
            "display": "C++",
            "outputs": ["{}.h"],
        },
    }

    # Input bdl files
    bdl_deps = depset([], transitive = [dep[BdlProvider].outputs for dep in ctx.attr.deps])

    # Generated outputs
    generated = {fmt: [] for fmt in formats.keys()}

    # Computet each input file independently
    for input_file in ctx.files.srcs:

        # Build the relative path of the input file from the BUILD file
        build_root_path = ctx.build_file_path.rsplit("/", 1)[0] + "/"
        relative_name = input_file.path.replace(build_root_path, "").replace(".bdl", "")

        # Preprocess the files here
        bdl_object = ctx.actions.declare_file("{}.bld.o".format(relative_name))
        ctx.actions.run(
            inputs = ctx.files.srcs + bdl_deps.to_list(),
            outputs = [bdl_object],
            progress_message = "Preprocess BDL manifest {}".format(input_file.short_path),
            arguments = ["--stage", "preprocess", "--output", bdl_object.path, input_file.path],
            executable = ctx.attr._bdl.files_to_run,
        )

        for fmt, data in formats.items():
            # Generate the output
            outputs = [ctx.actions.declare_file(output.format(relative_name)) for output in data["outputs"]]
            ctx.actions.run(
                inputs = [bdl_object] + bdl_deps.to_list(),
                outputs = outputs,
                progress_message = "Generating {} build files from manifest {}".format(data["display"], input_file.short_path),
                arguments = ["--stage", "generate", "--format", fmt, "--output", outputs[0].path, bdl_object.path],
                executable = ctx.attr._bdl.files_to_run,
            )
            # Save the outputs
            generated[fmt] += outputs

    # Generate the various providers
    cc_info_provider = _bzd_manifest_impl_cc(ctx = ctx, inputs = generated["cc"])
    cc_info_providers = cc_common.merge_cc_infos(cc_infos = [cc_info_provider] + [dep[CcInfo] for dep in ctx.attr.deps])

    return [
        BdlProvider(outputs=depset(ctx.files.srcs)),
        cc_info_providers
    ]

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
        "deps": attr.label_list(
            providers = [BdlProvider],
            doc = "List of bdl dependencies.",
        ),
        "_bdl": attr.label(
            default = Label("//tools/bdl"),
            cfg = "host",
            executable = True,
        ),
        "_deps_cc": attr.label_list(
            default = [Label("//tools/bdl/lang/cc/adapter:types")],
            cfg = "host",
        ),
    },
)
