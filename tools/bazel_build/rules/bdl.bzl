load("//tools/bazel_build/rules/assets/cc:defs.bzl", "cc_compile")

BdlTagProvider = provider(fields = [])
BdlProvider = provider(fields = ["bdls", "files"])
CcCompositionProvider = provider(fields = ["hdrs"])

def _get_cc_public_header(target):
    """Get all the direct public headers from a target."""

    if CcInfo not in target:
        return []
    return target[CcInfo].compilation_context.direct_public_headers

def _bdl_aspect_impl(target, ctx):
    """Aspects to gather all bdl depedency outputs."""

    # Are considered composition public headers when the target is not a BDL library but has a bdl library as a diret dependency.
    # Then it means it relies on a BDL interface.
    cc_hdrs = []
    if BdlTagProvider not in target:
        if any([dep for dep in ctx.rule.attr.deps if BdlTagProvider in dep]):
            cc_hdrs = _get_cc_public_header(target)

    cc_composition_provider = CcCompositionProvider(hdrs = depset(cc_hdrs, transitive = [dep[CcCompositionProvider].hdrs for dep in ctx.rule.attr.deps if CcCompositionProvider in dep]))

    if BdlProvider not in target:
        return [
            BdlProvider(
                files = depset(transitive = [dep[BdlProvider].files for dep in ctx.rule.attr.deps if BdlProvider in dep]),
                bdls = depset(transitive = [dep[BdlProvider].bdls for dep in ctx.rule.attr.deps if BdlProvider in dep]),
            ),
            cc_composition_provider,
        ]

    return [cc_composition_provider]

_bdl_aspect = aspect(
    implementation = _bdl_aspect_impl,
    attr_aspects = ["deps"],
)

def _make_bdl_arguments(ctx, stage, args):
    _PREPROCESS_FORMAT = "{}/{{}}.o".format(ctx.bin_dir.path)
    return ["--stage", stage, "--preprocess-format", _PREPROCESS_FORMAT] + args

def _bdl_library_impl(ctx):
    _FORMATS = {
        "cc": {
            "display": "C++",
            "outputs": ["{}.hh"],
            "includes_getter": _get_cc_public_header,
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
            # Build the list of the direct includes.
            includes = []
            for dep in ctx.attr.deps:
                includes.extend(data["includes_getter"](dep))
            arguments_includes = ["--include={}".format(include.short_path) for include in includes]

            # Generate the output
            outputs = [ctx.actions.declare_file(output.format(bdl["relative_name"])) for output in data["outputs"]]
            ctx.actions.run(
                inputs = depset(transitive = [files, bdls]),
                outputs = outputs,
                progress_message = "Generating {} build files from manifest {}".format(data["display"], bdl["input"].short_path),
                arguments = _make_bdl_arguments(ctx, "generate", arguments_includes + ["--format", fmt, "--output", outputs[0].path, bdl["input"].path]),
                executable = ctx.attr._bdl.files_to_run,
            )

            # Save the outputs
            generated[fmt] += outputs

    # Generate the various providers
    cc_info_provider = cc_compile(ctx = ctx, hdrs = generated["cc"], deps = ctx.attr._deps_cc + ctx.attr.deps)

    return [
        BdlProvider(bdls = bdls, files = files),
        BdlTagProvider(),
        cc_info_provider,
    ]

bdl_library = rule(
    implementation = _bdl_library_impl,
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
            providers = [BdlProvider, CcInfo],
            aspects = [_bdl_aspect],
            doc = "List of bdl dependencies. Language specifiic dependencies will have their public interface included in the generated file.",
        ),
        "_bdl": attr.label(
            default = Label("//tools/bdl"),
            cfg = "exec",
            executable = True,
        ),
        "_deps_cc": attr.label_list(
            default = [Label("//tools/bdl/generators/cc/adapter:types")],
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

def _bdl_composition_impl(ctx):
    files = depset(transitive = [dep[BdlProvider].files for dep in ctx.attr.deps if BdlProvider in dep])
    bdls = depset(transitive = [dep[BdlProvider].bdls for dep in ctx.attr.deps if BdlProvider in dep])

    cc_composition_hdrs = depset(transitive = [dep[CcCompositionProvider].hdrs for dep in ctx.attr.deps if CcCompositionProvider in dep])

    # If there is no files to process, ignore.
    if not files:
        return [CcInfo()]

    # Generate the list of files to be includes.
    arguments_includes = ["--include={}".format(hdrs.short_path) for hdrs in cc_composition_hdrs.to_list()]

    # Generate the output
    output = ctx.actions.declare_file("{}.main.cc".format(ctx.attr.name))
    ctx.actions.run(
        # files also needs to be passed into argument for debugging purpose, in order to display a nice error message.
        inputs = depset(transitive = [files, bdls]),
        outputs = [output],
        progress_message = "Generating C++ composition for {}".format(ctx.label),
        arguments = _make_bdl_arguments(ctx, "compose", ["--format", "cc", "--output", output.path] + arguments_includes + [bdl.path for bdl in files.to_list()]),
        executable = ctx.attr._bdl.files_to_run,
    )

    cc_info_provider = cc_compile(ctx = ctx, srcs = [output], deps = ctx.attr.deps)

    return [cc_info_provider, DefaultInfo(files = depset([output]))]

bdl_composition = rule(
    implementation = _bdl_composition_impl,
    doc = """Bzd Description Language generator rule for binaries.
    This generates all the glue to pull in bdl pieces together in a final binary.
    """,
    attrs = {
        "deps": attr.label_list(
            mandatory = True,
            aspects = [_bdl_aspect],
            doc = "List of dependencies.",
        ),
        "_bdl": attr.label(
            default = Label("//tools/bdl"),
            cfg = "exec",
            executable = True,
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
