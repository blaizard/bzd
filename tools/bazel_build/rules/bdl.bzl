load("@bazel_skylib//lib:new_sets.bzl", "sets")

BdlProvider = provider(fields = ["outputs", "bdls"])

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
    _FORMATS = {
        "cc": {
            "display": "C++",
            "outputs": ["{}.h"],
        },
    }

    _PREPROCESS_FORMAT = "{}/{{}}.o".format(ctx.bin_dir.path)

    # Input bdl files
    bdl_deps = depset(transitive = [dep[BdlProvider].outputs for dep in ctx.attr.deps]).to_list()

    # Output files
    metadata = []

    # Generated outputs
    generated = {fmt: [] for fmt in _FORMATS.keys()}

    # Compute each input file independently to build a list of metadata.
    for input_file in ctx.files.srcs:
        # Build the relative path of the input file from the BUILD file
        build_root_path = ctx.build_file_path.rsplit("/", 1)[0] + "/"
        relative_name = input_file.path.replace(build_root_path, "").replace(".bdl", "")

        # Create the output file
        output = ctx.actions.declare_file("{}.bdl.o".format(relative_name))

        # Build the object
        metadata.append({
            "output": output,
            "input": input_file,
            "relative_name": relative_name,
        })

    # Preprocess all input files at once
    ctx.actions.run(
        inputs = ctx.files.srcs + bdl_deps,
        outputs = [bdl["output"] for bdl in metadata],
        progress_message = "Preprocess BDL manifest(s) {}".format(", ".join([bdl["input"].short_path for bdl in metadata])),
        arguments = ["--stage", "preprocess", "--preprocess-format", _PREPROCESS_FORMAT] + [bdl["input"].path for bdl in metadata],
        executable = ctx.attr._bdl.files_to_run,
    )

    # Combine the output with the exsitng deps
    bdl_deps += [bdl["output"] for bdl in metadata]

    # Generate the format specific outputs
    for bdl in metadata:
        for fmt, data in _FORMATS.items():
            # Generate the output
            outputs = [ctx.actions.declare_file(output.format(bdl["relative_name"])) for output in data["outputs"]]
            ctx.actions.run(
                inputs = bdl_deps,
                outputs = outputs,
                progress_message = "Generating {} build files from manifest {}".format(data["display"], bdl["input"].short_path),
                arguments = ["--stage", "generate", "--format", fmt, "--output", outputs[0].path, "--preprocess-format", _PREPROCESS_FORMAT, bdl["input"].path],
                executable = ctx.attr._bdl.files_to_run,
            )

            # Save the outputs
            generated[fmt] += outputs

    # Generate the various providers
    cc_info_provider = _bzd_manifest_impl_cc(ctx = ctx, inputs = generated["cc"])
    cc_info_providers = cc_common.merge_cc_infos(cc_infos = [cc_info_provider] + [dep[CcInfo] for dep in ctx.attr.deps])

    return [
        BdlProvider(outputs = depset(bdl_deps)),
        cc_info_providers,
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
            default = [Label("//tools/bdl/generators/cc/adapter:types")],
            cfg = "host",
        ),
    },
)
