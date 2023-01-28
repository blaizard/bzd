load("//tools/bazel_build/rules/assets/cc:defs.bzl", "cc_compile", "cc_link")

BdlTagProvider = provider(fields = [])
BdlProvider = provider(
    doc = "Provider for bdl files.",
    fields = {
        "sources": "Pair of input and preprocessed bdl files.",
        "files": "All files, containts the same set of files as in `sources`, but uses a different format for convenience.",
    },
)
CcCompositionProvider = provider(fields = ["hdrs"])
_TargetProvider = provider(
    doc = "Provider for a target.",
    fields = {
        "compositions": "List of composition files for this target.",
        "deps": "List of dependencies for this target.",
        "platform": "Target platform for this target.",
        "language": "Language used for this target.",
    },
)
_BdlCcProvider = provider(
    doc = "Files used to commpile a CC binary.",
    fields = {
        "srcs": "Source files.",
        "deps": "Dependencies.",
    },
)

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
                sources = depset(transitive = [dep[BdlProvider].sources for dep in ctx.rule.attr.deps if BdlProvider in dep]),
                files = depset(transitive = [dep[BdlProvider].files for dep in ctx.rule.attr.deps if BdlProvider in dep]),
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

def _precompile_bdl(ctx, srcs, deps, output_dir = None):
    # Input files and bdls
    input_sources = depset(transitive = [dep[BdlProvider].sources for dep in deps])
    input_files = depset(srcs, transitive = [dep[BdlProvider].files for dep in deps])

    # Output files
    metadata = []

    # Compute each input file independently to build a list of metadata.
    for input_file in srcs:
        if output_dir == None:
            # Build the relative path of the input file from the BUILD file
            build_root_path = ctx.build_file_path.rsplit("/", 1)[0] + "/"
            relative_name = input_file.path.replace(build_root_path, "").replace(".bdl", "")
        else:
            relative_name = "{}/{}".format(output_dir, input_file.basename.replace(".bdl", ""))

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
        inputs = input_files,
        outputs = [bdl["output"] for bdl in metadata],
        progress_message = "Preprocess BDL manifest(s) {}".format(", ".join([bdl["input"].short_path for bdl in metadata])),
        arguments = _make_bdl_arguments(ctx, "preprocess", ["{}@{}".format(bdl["input"].path, bdl["output"].path) for bdl in metadata]),
        executable = ctx.attr._bdl.files_to_run,
    )

    sources = depset([(bdl["input"], bdl["output"]) for bdl in metadata], transitive = [input_sources])
    files = depset([bdl["output"] for bdl in metadata], transitive = [input_files])

    return BdlProvider(sources = sources, files = files), metadata

def _bdl_library_impl(ctx):
    _FORMATS = {
        "cc": {
            "display": "C++",
            "outputs": ["{}.hh"],
            "includes_getter": _get_cc_public_header,
        },
    }

    # Generated outputs
    generated = {fmt: [] for fmt in _FORMATS.keys()}

    # Pre-compile the BDLs into their language agnostics format.
    bdl_provider, metadata = _precompile_bdl(ctx, ctx.files.srcs, ctx.attr.deps)

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
                inputs = bdl_provider.files,
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
        bdl_provider,
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
            doc = "List of bdl dependencies. Language specific dependencies will have their public interface included in the generated file.",
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

def _make_composition_language_providers(ctx, name, deps, extra_bdl_provider = None):
    bdl_providers = [dep[BdlProvider] for dep in deps if BdlProvider in dep] + ([extra_bdl_provider] if extra_bdl_provider else [])
    sources = depset(transitive = [provider.sources for provider in bdl_providers])
    files = depset(transitive = [provider.files for provider in bdl_providers])

    cc_composition_hdrs = depset(transitive = [dep[CcCompositionProvider].hdrs for dep in deps if CcCompositionProvider in dep])

    # If there is no files to process, ignore.
    if not files:
        return [CcInfo()]

    # Generate the list of files to be includes.
    arguments_includes = ["--include={}".format(hdrs.short_path) for hdrs in cc_composition_hdrs.to_list()]

    # Generate the output
    output = ctx.actions.declare_file("{}.main.cc".format(name))
    ctx.actions.run(
        # files also needs to be passed into argument for debugging purpose, in order to display a nice error message.
        inputs = files,
        outputs = [output],
        progress_message = "Generating C++ composition for //{}:{}".format(ctx.label.package, name),
        arguments = _make_bdl_arguments(ctx, "compose", ["--format", "cc", "--output", output.path] + arguments_includes + ["{}@{}".format(source[0].path, source[1].path) for source in sources.to_list()]),
        executable = ctx.attr._bdl.files_to_run,
    )

    cc_info_provider = cc_compile(ctx = ctx, name = name, srcs = [output], deps = deps)

    return [cc_info_provider, DefaultInfo(files = depset([output])), _BdlCcProvider(srcs = [output], deps = deps)]

def _bdl_composition_impl(ctx):
    return _make_composition_language_providers(
        ctx = ctx,
        name = ctx.attr.name,
        deps = ctx.attr.deps,
    )

_COMMON_ATTRS = {
    "_bdl": attr.label(
        default = Label("//tools/bdl"),
        cfg = "exec",
        executable = True,
    ),
    "_cc_toolchain": attr.label(
        default = Label("@rules_cc//cc:current_cc_toolchain"),
    ),
}

bdl_composition = rule(
    implementation = _bdl_composition_impl,
    doc = """Bzd Description Language generator rule for binaries.
    This generates all the glue to pull in bdl pieces together in a final binary.
    """,
    attrs = dict({
        "deps": attr.label_list(
            mandatory = True,
            aspects = [_bdl_aspect],
            doc = "List of dependencies.",
        ),
    }, **_COMMON_ATTRS),
    toolchains = [
        "@rules_cc//cc:toolchain_type",
    ],
    fragments = ["cpp"],
)

def _bdl_system_impl(ctx):
    # Compose the target and their name into a dictionary.
    targets = {}
    for target, names in ctx.attr.targets.items():
        for name in names.split(","):
            if name in targets:
                fail("The target namespace '{}' is defined twice.".format(name))
            targets[name] = target

    # Loop through all the name/target pairs and generate the composition files.
    outputs = []
    for name, target in targets.items():
        target_provider = target[_TargetProvider]
        target_name = "{}.{}".format(ctx.label.name, name)

        # Generate the target-specific composition by injecting the new namespace.
        # Need to understand how to deal with the name.
        bdl_provider, _ = _precompile_bdl(ctx, target_provider.compositions, target_provider.deps, target_name)

        # Generate the composition.
        cc_info, default_info_provider, cc_provider = _make_composition_language_providers(
            ctx = ctx,
            name = target_name,
            deps = ctx.attr.deps + target_provider.deps,
            extra_bdl_provider = bdl_provider,
        )

        # C++
        if target_provider.language == "cc":
            binary_file, _ = cc_link(ctx, name = target_name + ".temp", srcs = cc_provider.srcs, deps = cc_provider.deps)
            outputs.append(binary_file)

        else:
            fail("Unsupported target language '{}'.".format(target_provider.language))

    return [DefaultInfo(files = depset(outputs))]

bdl_system = rule(
    implementation = _bdl_system_impl,
    doc = """Generate a system from targets.""",
    attrs = dict({
        "targets": attr.label_keyed_string_dict(
            mandatory = True,
            providers = [_TargetProvider],
            doc = "Targets to be included for the system definition.",
        ),
        "deps": attr.label_list(
            mandatory = True,
            doc = "List of dependencies.",
            aspects = [_bdl_aspect],
        ),
    }, **_COMMON_ATTRS),
    toolchains = [
        "@rules_cc//cc:toolchain_type",
    ],
    fragments = ["cpp"],
)

def _bdl_target_impl(ctx):
    return _TargetProvider(
        compositions = ctx.files.compositions,
        deps = ctx.attr.deps,
        platform = ctx.attr.platform,
        language = ctx.attr.language,
    )

bdl_target = rule(
    implementation = _bdl_target_impl,
    doc = """Target definition for the bzd framework.""",
    attrs = {
        "language": attr.string(
            mandatory = True,
            doc = "Compilation language for this rule.",
            values = ["cc"],
        ),
        "compositions": attr.label_list(
            mandatory = True,
            allow_files = [".bdl"],
            doc = "List of composition bdl source files for this target.",
        ),
        "deps": attr.label_list(
            doc = "List of dependencies.",
            aspects = [_bdl_aspect],
        ),
        "platform": attr.label(
            mandatory = True,
            doc = "The target platform for which this target should be compiled against.",
        ),
    },
)
