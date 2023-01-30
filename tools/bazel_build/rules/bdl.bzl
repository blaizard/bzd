load("//tools/bazel_build/rules/assets/cc:defs.bzl", "cc_compile", "cc_link")
load("@bazel_skylib//lib:sets.bzl", "sets")

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

def _make_bdl_arguments(ctx, stage, format = None, output = None, namespace = None, data = None, args = None):
    """Create the argument list for the `bdl` tool."""

    _PREPROCESS_FORMAT = "{}/{{}}.o".format(ctx.bin_dir.path)
    arguments = ["--stage", stage, "--preprocess-format", _PREPROCESS_FORMAT]
    if format:
        arguments += ["--format", format]
    if output:
        arguments += ["--output", output.path]
    if namespace:
        arguments += ["--namespace", namespace]
    if data:
        arguments += ["--data", data.path]
    if args:
        arguments += args
    return arguments

def _make_bdl_data_file(includes_per_target):
    """Create the language specific data file.

    Entries are keyed by language.
    """
    return {
        "cc": {
            "includes": {target: sets.to_list(sets.make([f.path for f in deps])) for target, deps in includes_per_target.items()},
        },
    }

def _precompile_bdl(ctx, srcs, deps, output_dir = None, namespace = None):
    """Precompile a set of bdls.

    Args:
        ctx: The context used for this action.
        srcs: The set of bdls to be precompiled.
        deps: The dependencies associated with these bdls, must have a `BdlProvider`.
        output_dir: The output directory where the precompiled objects show be stored,
                    if not specified, it will be stored in the same directory as the source file.
        namespace: The namespace in which the bdls files should be compiled.
    """

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
        arguments = _make_bdl_arguments(
            ctx = ctx,
            stage = "preprocess",
            namespace = namespace,
            args = ["{}@{}".format(bdl["input"].path, bdl["output"].path) for bdl in metadata],
        ),
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
        },
    }

    # Generated outputs
    generated = {fmt: [] for fmt in _FORMATS.keys()}

    # Pre-compile the BDLs into their language agnostics format.
    bdl_provider, metadata = _precompile_bdl(
        ctx = ctx,
        srcs = ctx.files.srcs,
        deps = ctx.attr.deps,
    )

    # Generate the format specific outputs
    for bdl in metadata:
        # Create the language specific data file.
        language_specific_data = _make_bdl_data_file(includes_per_target = {"": [f for dep in ctx.attr.deps for f in _get_cc_public_header(dep)]})

        # Write the language specific data file.
        data_file = ctx.actions.declare_file("{}.data.json".format(ctx.label.name, "{}.data.json".format(bdl["relative_name"])))
        ctx.actions.write(
            output = data_file,
            content = json.encode(language_specific_data),
        )

        for fmt, data in _FORMATS.items():
            # Generate the output
            outputs = [ctx.actions.declare_file(output.format(bdl["relative_name"])) for output in data["outputs"]]
            ctx.actions.run(
                inputs = depset([data_file], transitive = [bdl_provider.files]),
                outputs = outputs,
                progress_message = "Generating {} build files from manifest {}".format(data["display"], bdl["input"].short_path),
                arguments = _make_bdl_arguments(
                    ctx = ctx,
                    stage = "generate",
                    format = fmt,
                    output = outputs[0],
                    data = data_file,
                    args = [bdl["input"].path],
                ),
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

def _make_composition_language_providers(ctx, name, deps, target_deps = None, target_bdl_providers = None):
    """Compose the system.

    Args:
        ctx: The context of the action running the composition.
        name: The name of the generated output.
        deps: The dependencies associated with this action, it shall contain BdlProvider and lannguage specific providers
              such as CcInfo, etc.
        target_deps: Dictionary keyed by target name of the dependencies.
        target_bdl_providers: Dictionary keyed by target of extra BdlProvider to be added.
    """

    # Set the defaulf value for optional arguments.
    target_deps = target_deps if target_deps else {}
    target_bdl_providers = target_bdl_providers if target_bdl_providers else {}

    # Contains all BDL providers following this schema: List[Pair[BdlProvider, Optional[target]]]
    bdl_providers = [(dep[BdlProvider], None) for dep in deps if BdlProvider in dep] + [(provider, name) for name, provider in target_bdl_providers.items()]

    # Source list with the following format <path>@<precompiled>@<target> (where @<target> is optional)
    sources = ["{}@{}{}".format(source[0].path, source[1].path, ("@" + target) if target else "") for provider, target in bdl_providers for source in provider.sources.to_list()]

    # Contains all bdl files, this is used for debugging purpose.
    files = depset(transitive = [provider.files for provider, target in bdl_providers])

    # Contains all deps grouped by target.
    combined_deps = dict({None: deps}, **target_deps)

    # If there is no files to process, ignore.
    if not files:
        return [CcInfo()]

    # Create the language specific data file.
    def deps_to_cc_hdrs(deps):
        return [f for dep in deps if CcCompositionProvider in dep for f in dep[CcCompositionProvider].hdrs.to_list()]

    language_specific_data = _make_bdl_data_file(
        includes_per_target = dict({
            "": deps_to_cc_hdrs(deps),
        }, **{target: deps_to_cc_hdrs(deps) for target, deps in target_deps.items()}),
    )

    # Write the language specific data file.
    data_file = ctx.actions.declare_file("{}.data.json".format(name))
    ctx.actions.write(
        output = data_file,
        content = json.encode(language_specific_data),
    )

    # Generate the output
    output = ctx.actions.declare_file("{}.main.cc".format(name))
    ctx.actions.run(
        # files also needs to be passed into argument for debugging purpose, in order to display a nice error message.
        inputs = depset([data_file], transitive = [files]),
        outputs = [output],
        progress_message = "Generating C++ composition for //{}:{}".format(ctx.label.package, name),
        arguments = _make_bdl_arguments(
            ctx = ctx,
            stage = "compose",
            format = "cc",
            output = output,
            data = data_file,
            args = sources,
        ),
        executable = ctx.attr._bdl.files_to_run,
    )

    all_deps = [dep for target, deps in combined_deps.items() for dep in deps]
    cc_info_provider = cc_compile(ctx = ctx, name = name, srcs = [output], deps = all_deps)

    return [cc_info_provider, DefaultInfo(files = depset([output])), _BdlCcProvider(srcs = [output], deps = all_deps)]

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
    bdl_providers = {}
    deps = {}
    for name, target in targets.items():
        target_provider = target[_TargetProvider]
        target_name = "{}.{}".format(ctx.label.name, name)

        # Generate the target-specific composition by injecting the new namespace.
        bdl_provider, _ = _precompile_bdl(
            ctx = ctx,
            srcs = target_provider.compositions,
            deps = target_provider.deps,
            output_dir = target_name,
            namespace = name,
        )
        bdl_providers[name] = bdl_provider
        deps[name] = target_provider.deps

    # Generate the composition files.
    cc_info, default_info_provider, cc_provider = _make_composition_language_providers(
        ctx = ctx,
        name = ctx.attr.name,
        deps = ctx.attr.deps,
        target_deps = deps,
        target_bdl_providers = bdl_providers,
    )

    outputs = []
    for name, target in targets.items():
        target_provider = target[_TargetProvider]
        target_name = "{}.{}".format(ctx.label.name, name)

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
