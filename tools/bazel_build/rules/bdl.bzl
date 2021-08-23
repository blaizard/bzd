load("//tools/bazel_build/rules/assets/cc:defs.bzl", "cc_compile")

BdlProvider = provider(fields = ["bdls", "files"])

def _bzd_manifest_aspect_impl(target, ctx):
    """
    Aspects to gather all bdl depedency outputs.
    """
    if BdlProvider not in target:
        return [BdlProvider(
            files = depset(transitive = [dep[BdlProvider].files for dep in ctx.rule.attr.deps if BdlProvider in dep]),
            bdls = depset(transitive = [dep[BdlProvider].bdls for dep in ctx.rule.attr.deps if BdlProvider in dep]),
        )]
    return []

_bzd_manifest_aspect = aspect(
    implementation = _bzd_manifest_aspect_impl,
    attr_aspects = ["deps"],
)

def _make_bdl_arguments(ctx, stage, args):
    _PREPROCESS_FORMAT = "{}/{{}}.o".format(ctx.bin_dir.path)
    return ["--stage", stage, "--preprocess-format", _PREPROCESS_FORMAT] + args

def _bzd_manifest_impl(ctx):
    _FORMATS = {
        "cc": {
            "display": "C++",
            "outputs": ["{}.h"],
        },
    }

    # Input files and bdls
    files = depset(ctx.files.srcs, transitive = [dep[BdlProvider].files for dep in ctx.attr.deps])
    bdls = depset(transitive = [dep[BdlProvider].bdls for dep in ctx.attr.deps])

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

    # Preprocess all input files at once, this stage is language agnostic.
    ctx.actions.run(
        inputs = depset(transitive = [files, bdls]),
        outputs = [bdl["output"] for bdl in metadata],
        progress_message = "Preprocess BDL manifest(s) {}".format(", ".join([bdl["input"].short_path for bdl in metadata])),
        arguments = _make_bdl_arguments(ctx, "preprocess", [bdl["input"].path for bdl in metadata]),
        executable = ctx.attr._bdl.files_to_run,
    )

    # Combine the output with the exsitng deps
    bdls = depset([bdl["output"] for bdl in metadata], transitive = [bdls])

    # Generate the format specific outputs
    for bdl in metadata:
        for fmt, data in _FORMATS.items():
            # Generate the output
            outputs = [ctx.actions.declare_file(output.format(bdl["relative_name"])) for output in data["outputs"]]
            ctx.actions.run(
                inputs = depset(transitive = [files, bdls]),
                outputs = outputs,
                progress_message = "Generating {} build files from manifest {}".format(data["display"], bdl["input"].short_path),
                arguments = _make_bdl_arguments(ctx, "generate", ["--format", fmt, "--output", outputs[0].path, bdl["input"].path]),
                executable = ctx.attr._bdl.files_to_run,
            )

            # Save the outputs
            generated[fmt] += outputs

    # Generate the various providers
    cc_info_provider = cc_compile(ctx = ctx, hdrs = generated["cc"], deps = ctx.attr._deps_cc + ctx.attr.deps)

    return [
        BdlProvider(bdls = bdls, files = files),
        cc_info_provider,
    ]

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
        "_cc_toolchain": attr.label(
            default = Label("@rules_cc//cc:current_cc_toolchain"),
        ),
    },
    toolchains = [
        "@rules_cc//cc:toolchain_type",
    ],
    fragments = ["cpp"],
)

def _bzd_manifest_binary_impl(ctx):
    files = depset(transitive = [dep[BdlProvider].files for dep in ctx.attr.deps if BdlProvider in dep])
    bdls = depset(transitive = [dep[BdlProvider].bdls for dep in ctx.attr.deps if BdlProvider in dep])

    # If there is no files to process, ignore.
    if not files:
        return [CcInfo()]

    # Generate the output
    output = ctx.actions.declare_file("output.h")
    ctx.actions.run(
        # files needs to be passed into argument for debugging purpose, in order to display a nice error message.
        inputs = depset(transitive = [files, bdls]),
        outputs = [output],
        progress_message = "Generating C++ composition",
        arguments = _make_bdl_arguments(ctx, "compose", ["--format", "cc", "--output", output.path] + [bdl.path for bdl in files.to_list()]),
        executable = ctx.attr._bdl.files_to_run,
    )

    compilation_context = cc_common.create_compilation_context(
        headers = depset([output]),
    )

    return [CcInfo(compilation_context = compilation_context), DefaultInfo(files = depset([output]))]

bzd_manifest_binary = rule(
    implementation = _bzd_manifest_binary_impl,
    doc = """Bzd Description Language generator rule for binaries.
    This generates all the glue to pull in bdl pieces together in a final binary.
    """,
    attrs = {
        "deps": attr.label_list(
            mandatory = True,
            aspects = [_bzd_manifest_aspect],
            doc = "List of dependencies.",
        ),
        "_bdl": attr.label(
            default = Label("//tools/bdl"),
            cfg = "host",
            executable = True,
        ),
    },
)
