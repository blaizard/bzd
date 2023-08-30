"""BDL rules."""

load("@bazel_skylib//lib:sets.bzl", "sets")
load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")
load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")
load("@bzd_package//:defs.bzl", "BzdPackageMetadataFragmentInfo")
load("@bzd_toolchain_cc//cc:defs.bzl", "cc_compile", "cc_link")

# ---- Providers ----

_BdlTagInfo = provider(
    doc = "Empty provider to identify a BDL library.",
    fields = [],
)

_BdlInfo = provider(
    doc = "Provider for bdl files.",
    fields = {
        "files": "All files, containts the same set of files as in `sources`, but uses a different format for convenience.",
        "search_formats": "Set of search formats string to locate the preprocessed object files.",
        "sources": "Pair of input and preprocessed bdl files.",
    },
)

_BdlTargetInfo = provider(
    doc = "Provider for a target.",
    fields = {
        "composition": "List of composition files for this target.",
        "deps": "List of dependencies for this target.",
        "language": "Language used for this target.",
    },
)

_BdlSystemInfo = provider(
    doc = "Provider for a system rule.",
    fields = {
        "cc": "A set of _BdlCcInfo keyed by target names.",
    },
)

_BdlCcInfo = provider(
    doc = "Files used to commpile a CC binary.",
    fields = {
        "deps": "Dependencies.",
        "srcs": "Source files.",
    },
)

_BdlCcCompositionInfo = provider(
    doc = "Provider to gather public header files from cc rules.",
    fields = {
        "hdrs": "list of public header files.",
    },
)

# ---- Aspects ----

def _get_cc_public_header(target):
    """Get all the direct public headers from a target."""

    if CcInfo not in target:
        return []
    return target[CcInfo].compilation_context.direct_public_headers

def _aspect_bdl_providers_impl(target, ctx):
    """Aspects to gather all bdl depedency outputs."""

    # Are considered composition public headers when the target is not a BDL library but has a bdl library as a diret dependency.
    # Then it means it relies on a BDL interface.
    cc_hdrs = []
    if _BdlTagInfo not in target:
        if any([dep for dep in ctx.rule.attr.deps if _BdlTagInfo in dep]):
            cc_hdrs = _get_cc_public_header(target)

    cc_composition_provider = _BdlCcCompositionInfo(hdrs = depset(cc_hdrs, transitive = [dep[_BdlCcCompositionInfo].hdrs for dep in ctx.rule.attr.deps if _BdlCcCompositionInfo in dep]))

    if _BdlInfo not in target:
        return [
            _BdlInfo(
                sources = depset(transitive = [dep[_BdlInfo].sources for dep in ctx.rule.attr.deps if _BdlInfo in dep]),
                files = depset(transitive = [dep[_BdlInfo].files for dep in ctx.rule.attr.deps if _BdlInfo in dep]),
                search_formats = sets.to_list(sets.make([d for dep in ctx.rule.attr.deps if _BdlInfo in dep for d in dep[_BdlInfo].search_formats])),
            ),
            cc_composition_provider,
        ]

    return [cc_composition_provider]

_aspect_bdl_providers = aspect(
    implementation = _aspect_bdl_providers_impl,
    attr_aspects = ["deps"],
)

# ---- Transisitions ----

def _transition_platform_impl(_settings, attr):
    if not hasattr(attr, "platform"):
        fail("This rule does not contain a valid platform attribute.")
    if not attr.platform:
        return {}
    return {
        "//command_line_option:platforms": str(attr.platform),
    }

_transition_platform = transition(
    implementation = _transition_platform_impl,
    inputs = [],
    outputs = [
        "//command_line_option:platforms",
    ],
)

# ---- Helpers for bdl CLI ----

def _get_preprocessed_format(ctx):
    return "{}/{{}}.o".format(ctx.bin_dir.path)

def _make_bdl_arguments(ctx, stage, search_formats = None, format = None, output = None, namespace = None, data = None, targets = None, args = None):
    """Create the argument list for the `bdl` tool."""

    arguments = ["--stage", stage, "--preprocess-format", _get_preprocessed_format(ctx)]
    if search_formats:
        arguments += [i for fmt in search_formats for i in ("--search-format", fmt)]
    if format:
        arguments += ["--format", format]
    if output:
        arguments += ["--output", output]
    if namespace:
        arguments += ["--namespace", namespace]
    if data:
        arguments += ["--data", data.path]
    if targets:
        arguments += [i for target in targets if target for i in ("--target", target)]
    if args:
        arguments += args
    return arguments

def _make_bdl_data_file(includes_per_target):
    """Create the language specific data file.

    Entries are keyed by language.
    """
    return {
        "cc": {
            "includes": {target: sets.to_list(sets.make([f.short_path for f in deps])) for target, deps in includes_per_target.items()},
        },
    }

# ---- Rules ----

def _precompile_bdl(ctx, srcs, deps, output_dir = None, namespace = None):
    """Precompile a set of bdls.

    Args:
        ctx: The context used for this action.
        srcs: The set of bdls to be precompiled.
        deps: The dependencies associated with these bdls, must have a `_BdlInfo`.
        output_dir: The output directory where the precompiled objects show be stored,
                    if not specified, it will be stored in the same directory as the source file.
        namespace: The namespace in which the bdls files should be compiled.
    """

    # Input files and bdls
    input_sources = depset(transitive = [dep[_BdlInfo].sources for dep in deps])
    input_files = depset(srcs, transitive = [dep[_BdlInfo].files for dep in deps])
    search_formats = sets.make([d for dep in deps for d in dep[_BdlInfo].search_formats])

    # Output files
    metadata = []

    # Compute each input file independently to build a list of metadata.
    for input_file in srcs:
        if output_dir == None:
            # Build the relative path of the input file from the BUILD file
            build_root_path = ctx.build_file_path.rsplit("/", 1)[0] + "/"
            relative_name = input_file.path.replace(build_root_path, "").replace(".bdl", "")
        else:
            relative_name = "{}/{}/{}".format(output_dir, input_file.dirname, input_file.basename.replace(".bdl", ""))

        # Create the output file
        output = ctx.actions.declare_file("{}.bdl.o".format(relative_name))

        # Build the object
        metadata.append({
            "input": input_file,
            "output": output,
            "relative_name": relative_name,
        })

    # Add the new path to the list, it must be done before preprocessing the files,
    # as some might refer to others.
    sets.insert(search_formats, _get_preprocessed_format(ctx))
    search_formats = sets.to_list(search_formats)

    # Preprocess all input files at once, this stage is language agnostic.
    ctx.actions.run(
        inputs = input_files,
        outputs = [bdl["output"] for bdl in metadata],
        progress_message = "Preprocessing BDL manifest(s) {}".format(", ".join([bdl["input"].short_path for bdl in metadata])),
        arguments = _make_bdl_arguments(
            ctx = ctx,
            stage = "preprocess",
            namespace = namespace,
            search_formats = search_formats,
            args = ["{}@{}".format(bdl["input"].path, bdl["output"].path) for bdl in metadata],
        ),
        executable = ctx.attr._bdl.files_to_run,
    )

    sources = depset([(bdl["input"], bdl["output"]) for bdl in metadata], transitive = [input_sources])
    files = depset([bdl["output"] for bdl in metadata], transitive = [input_files])

    return _BdlInfo(sources = sources, files = files, search_formats = search_formats), metadata

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
                    search_formats = bdl_provider.search_formats,
                    format = fmt,
                    output = outputs[0].path,
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
        _BdlTagInfo(),
        cc_info_provider,
    ]

bdl_library = rule(
    implementation = _bdl_library_impl,
    doc = """Bzd Description Language generator rule.
    It generates language provider from a .bdl file.
    The files are generated at the same path of the target, with the name of the target appended with a language specific file extension.
    """,
    attrs = {
        "deps": attr.label_list(
            providers = [_BdlInfo, CcInfo],
            aspects = [_aspect_bdl_providers],
            doc = "List of bdl dependencies. Language specific dependencies will have their public interface included in the generated file.",
        ),
        "srcs": attr.label_list(
            mandatory = True,
            allow_files = [".bdl"],
            doc = "List of Bzd Description Language (bdl) files to be included.",
        ),
        "_bdl": attr.label(
            default = Label("//tools/bdl"),
            cfg = "exec",
            executable = True,
        ),
        "_cc_toolchain": attr.label(
            default = Label("@rules_cc//cc:current_cc_toolchain"),
        ),
        "_deps_cc": attr.label_list(
            default = [Label("//tools/bdl/generators/cc/adapter:types")],
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
        deps: The dependencies associated with this action, it shall contain _BdlInfo and lannguage specific providers
              such as CcInfo, etc.
        target_deps: Dictionary keyed by target name of the dependencies.
        target_bdl_providers: Dictionary keyed by target of extra _BdlInfo to be added.
    """

    # Set the defaulf value for optional arguments.
    target_deps = target_deps if target_deps else {}
    target_bdl_providers = target_bdl_providers if target_bdl_providers else {}

    # Contains all BDL providers following this schema: List[Pair[_BdlInfo, Optional[target]]]
    bdl_providers = [(dep[_BdlInfo], None) for dep in deps if _BdlInfo in dep] + [(provider, name) for name, provider in target_bdl_providers.items()]

    # Source list with the following format <path>@<precompiled>@<target> (where @<target> is optional)
    sources = ["{}@{}".format(source[0].path, source[1].path) for provider, target in bdl_providers for source in provider.sources.to_list()]

    # Contains all bdl files, this is used for debugging purpose.
    files = depset(transitive = [provider.files for provider, target in bdl_providers])

    # Contains all search format strings.
    search_formats = sets.to_list(sets.make([fmt for provider, target in bdl_providers for fmt in provider.search_formats]))

    # Contains all deps grouped by target.
    combined_deps = dict({"all": deps}, **target_deps)

    # List all targets
    targets = [target for target in target_bdl_providers.keys()]

    # If there is no files to process, ignore.
    if not files:
        return [CcInfo()]

    # Create the language specific data file.
    def deps_to_cc_hdrs(deps):
        return [f for dep in deps if _BdlCcCompositionInfo in dep for f in dep[_BdlCcCompositionInfo].hdrs.to_list()]

    language_specific_data = _make_bdl_data_file(
        includes_per_target = dict({
            "all": deps_to_cc_hdrs(deps),
        }, **{target: deps_to_cc_hdrs(deps) for target, deps in target_deps.items()}),
    )

    # Write the language specific data file.
    data_file = ctx.actions.declare_file("{}.data.json".format(name))
    ctx.actions.write(
        output = data_file,
        content = json.encode(language_specific_data),
    )

    # Generate the expected output files.
    outputs = {target: ctx.actions.declare_file("{}.composition.{}.cc".format(name, target)) for target in (targets if targets else ["all"])}

    ctx.actions.run(
        # files also needs to be passed into argument for debugging purpose, in order to display a nice error message.
        inputs = depset([data_file], transitive = [files]),
        outputs = outputs.values(),
        progress_message = "Generating C++ composition for //{}:{}".format(ctx.label.package, name),
        arguments = _make_bdl_arguments(
            ctx = ctx,
            stage = "compose",
            format = "cc",
            search_formats = search_formats,
            targets = targets,
            output = "{}/{}.composition".format(outputs.values()[0].dirname, name),
            data = data_file,
            args = sources,
        ),
        executable = ctx.attr._bdl.files_to_run,
    )

    cc_providers = {}
    for target, output in outputs.items():
        deps = combined_deps["all"] + ([] if target == "all" else combined_deps.get(target, [])) + ctx.attr._deps_cc
        cc_providers[target] = _BdlCcInfo(srcs = [output], deps = deps)

    return cc_providers

def _bdl_binary_build(ctx, name, binary_file):
    """Prepare the binary for the execution stage.

    Args:
        ctx: The context of the action.
        name: The name of the output.
        binary_file: The binary of language-specific rule.

    Returns:
        A list of binaries and metadatas
    """

    binary_toolchain = ctx.toolchains["@bzd_toolchain_cc//binary:toolchain_type"].info
    binaries = [binary_file]
    metadatas = []

    # Run the build steps
    for index, build in enumerate(binary_toolchain.build):
        build_binary_file = ctx.actions.declare_file("{}.build.{}".format(name, index))
        ctx.actions.run(
            inputs = [binary_file],
            outputs = [build_binary_file],
            tools = build.data_runfiles.files,
            arguments = [binary_file.path, build_binary_file.path],
            executable = build.files_to_run,
        )
        binaries.append(build_binary_file)

    # Run the metadata steps
    for index, metadata in enumerate(binary_toolchain.metadata):
        metadata_file = ctx.actions.declare_file("{}.metadata.{}".format(name, index))
        ctx.actions.run(
            inputs = binaries,
            outputs = [metadata_file],
            tools = metadata.default_runfiles.files,
            arguments = [b.path for b in binaries] + [metadata_file.path],
            executable = metadata.files_to_run,
        )
        metadatas.append(metadata_file)

    return binaries, metadatas

def _bdl_binary_execution(ctx, binaries):
    """Build the executor provider."""

    binary_toolchain = ctx.toolchains["@bzd_toolchain_cc//binary:toolchain_type"].info

    # Identify the executor
    executor = ctx.attr._executor[BuildSettingInfo].value
    executors_mapping = {value: key for key, values in binary_toolchain.executors.items() for value in values.split(",")}
    if executor not in executors_mapping:
        fail("This platform does not support the '{}' executor, only the followings are supported: {}.".format(executor, executors_mapping.keys()))

    args = []
    if ctx.attr._debug[BuildSettingInfo].value:
        args.append("--debug")
    args += [binary.short_path for binary in binaries]

    return sh_binary_wrapper_impl(
        ctx = ctx,
        binary = executors_mapping[executor],
        output = ctx.outputs.executable,
        data = binaries,
        command = "{{binary}} {} $@".format(" ".join(args)),
    ), []

def _bdl_system_impl(ctx):
    # Compose the target and their name into a dictionary.
    targets = {}
    for target, names in ctx.attr.targets.items():
        for name in names.split(","):
            if name in ("all", ""):
                fail("The target name '{}' is protected and therefore cannot be used.".format(name))
            if name in targets:
                fail("The target name '{}' is defined twice.".format(name))
            targets[name] = target

    # Loop through all the name/target pairs and generate the composition files.
    bdl_providers = {}
    deps = {}
    for name, target in targets.items():
        target_provider = target[_BdlTargetInfo]
        target_name = "{}.{}".format(ctx.label.name, name)

        # Generate the target-specific composition by injecting the new namespace.
        bdl_provider, _ = _precompile_bdl(
            ctx = ctx,
            srcs = target_provider.composition,
            deps = target_provider.deps,
            output_dir = target_name,
            namespace = name,
        )
        bdl_providers[name] = bdl_provider
        deps[name] = target_provider.deps

    # Generate the composition files.
    cc_providers = _make_composition_language_providers(
        ctx = ctx,
        name = ctx.attr.name,
        deps = ctx.attr.deps,
        target_deps = deps,
        target_bdl_providers = bdl_providers,
    )

    return _BdlSystemInfo(
        cc = cc_providers,
    )

_bdl_system = rule(
    implementation = _bdl_system_impl,
    doc = """Generate a system from targets.""",
    attrs = {
        "deps": attr.label_list(
            mandatory = True,
            doc = "List of dependencies.",
            aspects = [_aspect_bdl_providers],
        ),
        "targets": attr.label_keyed_string_dict(
            mandatory = True,
            providers = [_BdlTargetInfo],
            doc = "Targets to be included for the system definition.",
        ),
        "_bdl": attr.label(
            default = Label("//tools/bdl"),
            cfg = "exec",
            executable = True,
        ),
        "_cc_toolchain": attr.label(
            default = Label("@rules_cc//cc:current_cc_toolchain"),
        ),
        "_deps_cc": attr.label_list(
            default = [Label("//tools/bdl/generators/cc/adapter:context")],
        ),
    },
)

def _bdl_binary_impl(ctx):
    target_provider = ctx.attr.target[_BdlTargetInfo]
    system = ctx.attr.system[_BdlSystemInfo]
    name = ctx.label.name

    # C++
    if target_provider.language == "cc":
        cc_provider = system.cc[ctx.attr.target_name]
        binary_file, metadata_files = cc_link(ctx, name = name, srcs = cc_provider.srcs, deps = cc_provider.deps, map_analyzer = ctx.attr._map_analyzer_script)
        metadata = ctx.files._cc_metadata
        extra_providers = [coverage_common.instrumented_files_info(
            ctx,
            dependency_attributes = ["deps"],
        )]

    else:
        fail("Unsupported target language '{}'.".format(target_provider.language))

    # Build the binaries.
    binaries, build_metadata = _bdl_binary_build(ctx = ctx, name = name, binary_file = binary_file)

    # Add the execution wrapper.
    default_info, execution_metadata = _bdl_binary_execution(ctx = ctx, binaries = binaries)

    return [
        default_info,
        BzdPackageMetadataFragmentInfo(
            manifests = metadata_files + metadata + build_metadata + execution_metadata,
        ),
        OutputGroupInfo(metadata = metadata_files + metadata + build_metadata + execution_metadata),
    ] + extra_providers

def _bzd_binary_generic(is_test):
    return rule(
        implementation = _bdl_binary_impl,
        doc = """Create a binary from a system rule.""",
        attrs = {
            "platform": attr.label(
                default = None,
                doc = "The platform used for the transition of this rule.",
            ),
            "system": attr.label(
                mandatory = True,
                doc = "The system rule associated with this target.",
                providers = [_BdlSystemInfo],
            ),
            "target": attr.label(
                mandatory = True,
                doc = "The target label for this binary.",
                providers = [_BdlTargetInfo],
            ),
            "target_name": attr.string(
                mandatory = True,
                doc = "The name of the target.",
            ),
            "_allowlist_function_transition": attr.label(
                default = "@bazel_tools//tools/allowlists/function_transition_allowlist",
            ),
            "_cc_metadata": attr.label_list(
                default = [
                    Label("//tools/bazel_build/rules/assets/cc:metadata_json"),
                ],
                allow_files = True,
            ),
            "_debug": attr.label(
                default = "//tools/bazel_build/settings/debug",
            ),
            "_executor": attr.label(
                default = "//tools/bazel_build/settings/executor",
            ),
            "_map_analyzer_script": attr.label(
                executable = True,
                cfg = "exec",
                default = Label("@bzd_toolchain_cc//binary/map_analyzer"),
            ),
        },
        toolchains = [
            "@rules_cc//cc:toolchain_type",
            "@bzd_toolchain_cc//binary:toolchain_type",
        ],
        fragments = ["cpp"],
        cfg = _transition_platform,
        executable = True,
        test = is_test,
    )

_bdl_binary = _bzd_binary_generic(is_test = False)
_bdl_test = _bzd_binary_generic(is_test = True)

def _bdl_system_generic(is_test, name, targets, testonly, deps, **kwargs):
    """Create a system rule.

    A system rule create a composition and build all related binary together.

    Args:
        is_test: If this is a test or not.
        name: The name of the system.
        targets: a dictionary with name and target corresponding to the binaries
                for this system.
        testonly: Set testonly attribute.
        deps: Dependencies for the rule.
        **kwargs: Additional attributes to pass to the rule.
    """

    def _target_to_platform(target):
        """Convert a target label into its related platform label."""
        if Label(target).name.endswith("auto"):
            return None
        return str(Label(target)) + ".platform"

    targets_processed = {}
    for target_name, target in targets.items():
        if target in targets_processed:
            targets_processed[target] += "," + target_name
        else:
            targets_processed[target] = target_name

    _bdl_system(
        name = "{}.system".format(name),
        targets = targets_processed,
        testonly = testonly,
        deps = deps,
        tags = ["manual"],
    )

    binary_rule = _bdl_test if is_test else _bdl_binary

    for target_name, target in targets.items():
        binary_rule(
            name = "{}.{}".format(name, target_name) if len(targets) > 1 else name,
            platform = _target_to_platform(target),
            target = target,
            target_name = target_name,
            testonly = testonly,
            system = "{}.system".format(name),
            **kwargs
        )

def bdl_system(name, targets, deps = None, testonly = None, **kwargs):
    _bdl_system_generic(is_test = False, name = name, targets = targets, deps = deps, testonly = False if testonly == None else testonly, **kwargs)

def bdl_system_test(name, targets, deps = None, testonly = True, **kwargs):
    _bdl_system_generic(is_test = True, name = name, targets = targets, deps = deps, testonly = True if testonly == None else testonly, **kwargs)

def _bdl_target_impl(ctx):
    return _BdlTargetInfo(
        composition = ctx.files.composition,
        deps = ctx.attr.deps,
        language = ctx.attr.language,
    )

_bdl_target = rule(
    implementation = _bdl_target_impl,
    doc = """Target definition for the bzd framework.""",
    attrs = {
        "composition": attr.label_list(
            mandatory = True,
            allow_files = [".bdl"],
            doc = "List of composition bdl source files for this target.",
        ),
        "deps": attr.label_list(
            doc = "List of dependencies.",
            aspects = [_aspect_bdl_providers],
        ),
        "language": attr.string(
            mandatory = True,
            doc = "Compilation language for this rule.",
            values = ["cc"],
        ),
    },
)

def bdl_target(name, **kwargs):
    if not name.endswith("auto"):
        fail("The name for bdl_target must ends with 'auto', this is to tell the rule that it does not have related platform.")
    _bdl_target(
        name = name,
        **kwargs
    )

def _bdl_target_platform_impl(ctx):
    return ctx.attr.target[_BdlTargetInfo]

_bdl_target_platform = rule(
    implementation = _bdl_target_platform_impl,
    doc = """Target definition specialized for a specific platform for the bzd framework.""",
    attrs = {
        "platform": attr.label(
            mandatory = True,
            doc = "The platform associated with this target platform.",
        ),
        "target": attr.label(
            mandatory = True,
            providers = [_BdlTargetInfo],
            doc = "The target associated with this target platform.",
        ),
        "_allowlist_function_transition": attr.label(
            default = "@bazel_tools//tools/allowlists/function_transition_allowlist",
        ),
    },
    cfg = _transition_platform,
)

def bdl_target_platform(name, target, platform, visibility = None, tags = None):
    # This is the main target rule to this target. The relation between this
    # and the platform are through their name.
    _bdl_target_platform(
        name = name,
        target = target,
        platform = platform,
        visibility = visibility,
        tags = tags,
    )

    # A platform should be used here because we need to hardcode the name of the platform,
    # to be able to use it into the bazel transition.
    native.alias(
        name = "{}.platform".format(name),
        actual = platform,
        visibility = visibility,
        tags = tags,
    )
