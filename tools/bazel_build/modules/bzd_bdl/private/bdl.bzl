"""BDL rules."""

load("@bazel_skylib//lib:sets.bzl", "sets")
load("@bdl_extension//:extensions.bzl", "extensions")
load("@bzd_package//:defs.bzl", "BzdPackageMetadataFragmentInfo")
load("//extensions:json.bzl", extension_json = "extension")

visibility(["//..."])

# ---- Extensions ----

_library_extensions = {} | extension_json | extensions

# ---- Providers ----

_BdlTagInfo = provider(
    doc = "Empty provider to identify a BDL library.",
    fields = [],
)

_BdlInfo = provider(
    doc = "Provider for bdl files.",
    fields = {
        "files": "All files, contains the same set of files as in `sources`, but uses a different format for convenience.",
        "search_formats": "Set of search formats string to locate the preprocessed object files.",
        "sources": "Pair of input and preprocessed bdl files.",
    },
)

_BdlTargetInfo = provider(
    doc = "Provider for a target.",
    fields = {
        "binary": "Binary target.",
        "composition": "List of composition files for this target.",
        "deps": "List of dependencies for this target.",
        "language": "Language used for this target.",
    },
)

_BdlSystemInfo = provider(
    doc = "Provider for a system rule.",
    fields = {
        "data": "Data specific keyed by extension.",
    },
)

_BdlCompositionInfo = provider(
    doc = "Provider to gather language specific data for composition.",
    fields = {
        "data": "Data specific keyed by extension.",
    },
)

BdlSystemJsonInfo = provider(
    doc = "Provider for a system json representation of a system.",
    fields = {
        "json": "Dictionary of json files representing the system, keyed by target.",
    },
)

# ---- Aspects ----

def _aspect_bdl_providers_impl(target, ctx):
    """Aspects to gather all bdl dependency outputs."""

    # Are considered composition public headers when the target is not a BDL library but has a bdl library as a direct dependency.
    # Then it means it relies on a BDL interface.
    has_deps = hasattr(ctx.rule.attr, "deps")
    is_direct = (_BdlTagInfo not in target) and has_deps and (any([dep for dep in ctx.rule.attr.deps if _BdlTagInfo in dep]))
    transitive_composition = [dep[_BdlCompositionInfo] for dep in ctx.rule.attr.deps if _BdlCompositionInfo in dep] if has_deps else []
    provider_data = {fmt: {} for fmt in _library_extensions.keys()}
    for fmt, data in _library_extensions.items():
        if "aspect_files" in data:
            for key, aspect in data["aspect_files"].items():
                data = aspect(target) if is_direct else []
                provider_data[fmt][key] = depset(data, transitive = [t.data[fmt].get(key, depset()) for t in transitive_composition])

    if _BdlInfo not in target and has_deps:
        return [
            _BdlInfo(
                sources = depset(transitive = [dep[_BdlInfo].sources for dep in ctx.rule.attr.deps if _BdlInfo in dep]),
                files = depset(transitive = [dep[_BdlInfo].files for dep in ctx.rule.attr.deps if _BdlInfo in dep]),
                search_formats = sets.to_list(sets.make([d for dep in ctx.rule.attr.deps if _BdlInfo in dep for d in dep[_BdlInfo].search_formats])),
            ),
            _BdlCompositionInfo(data = provider_data),
        ]

    return [_BdlCompositionInfo(data = provider_data)]

_aspect_bdl_providers = aspect(
    implementation = _aspect_bdl_providers_impl,
    attr_aspects = ["deps"],
)

# ---- Transitions ----

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

# ---- Rules ----

def _precompile_bdl(ctx, srcs, deps, output_dir = None, namespace = None):
    """Precompile a set of bdls.

    Args:
        ctx: The context used for this action.
        srcs: The set of bdls to be precompiled.
        deps: The dependencies associated with these bdls, must have a `_BdlInfo`.
        output_dir: The output directory where the precompiled objects should be stored,
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
            relative_name = input_file.short_path.replace(build_root_path, "").replace(".bdl", "")
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
    # Pre-compile the BDLs into their language agnostics format.
    bdl_provider, metadata = _precompile_bdl(
        ctx = ctx,
        srcs = ctx.files.srcs,
        deps = ctx.attr.deps,
    )

    # Generate the data for the data file for the bdl constructor.
    language_specific_data = {}
    for fmt, data in _library_extensions.items():
        if "library" in data:
            language_specific_data[fmt] = data["library"]["data"](ctx.attr.deps)

    # Write the language specific data file.
    data_file = ctx.actions.declare_file("{}.data.json".format(ctx.label.name))
    ctx.actions.write(
        output = data_file,
        content = json.encode(language_specific_data),
    )

    # Generate the various providers
    providers = []
    for fmt, data in _library_extensions.items():
        if "library" in data:
            generated = []
            for bdl in metadata:
                # Generate the output
                outputs = [ctx.actions.declare_file(output.format(name = bdl["relative_name"])) for output in data["library"]["outputs"]]
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
                generated += outputs

            # Generate the various providers
            providers.extend(data["library"]["providers"](ctx = ctx, generated = generated))

    return [
        bdl_provider,
        _BdlTagInfo(),
    ] + providers

bdl_library = rule(
    implementation = _bdl_library_impl,
    doc = """Bzd Description Language generator rule.
    It generates language provider from a .bdl file.
    The files are generated at the same path of the target, with the name of the target appended with a language specific file extension.
    """,
    attrs = {
        "deps": attr.label_list(
            providers = [_BdlInfo],
            aspects = [_aspect_bdl_providers],
            doc = "List of bdl dependencies. Language specific dependencies will have their public interface included in the generated file.",
        ),
        "srcs": attr.label_list(
            mandatory = True,
            allow_files = [".bdl"],
            doc = "List of Bzd Description Language (bdl) files to be included.",
        ),
        "_bdl": attr.label(
            default = Label("//bdl"),
            cfg = "exec",
            executable = True,
        ),
    } | {("_deps_" + name): (attr.label_list(
        default = data["library"]["deps"],
    )) for name, data in _library_extensions.items() if "library" in data},
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
        deps: The dependencies associated with this action, it shall contain _BdlInfo and language specific providers
              such as CcInfo, etc.
        target_deps: Dictionary keyed by target name of the dependencies.
        target_bdl_providers: Dictionary keyed by target of extra _BdlInfo to be added.

    Returns:
        Returns provider per format and target in the following form:
        {
            fmt: {
                target: {
                    str1: ...,
                    str2: ....
                }
            }
        }
        for example:
        {
            json: {
                machine1: {
                    hello: file1
                }
            }
        }
    """

    # Set the default value for optional arguments.
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
    def deps_to_info(fmt, deps):
        return [dep[_BdlCompositionInfo].data.get(fmt, {}) for dep in deps if _BdlCompositionInfo in dep]

    language_specific_data = {}
    for fmt, data in _library_extensions.items():
        if "composition" in data:
            language_specific_data[fmt] = data["composition"]["data"](deps_to_info(fmt, deps), {target: deps_to_info(fmt, deps) for target, deps in target_deps.items()})

    # Write the language specific data file.
    data_file = ctx.actions.declare_file("{}.data.json".format(name))
    ctx.actions.write(
        output = data_file,
        content = json.encode(language_specific_data),
    )

    system_providers = {}
    for fmt, data in _library_extensions.items():
        if "composition" in data:
            # Generate the expected output files.
            outputs = {target: ctx.actions.declare_file(data["composition"]["output"].format(name = name, target = target)) for target in (targets if targets else ["all"])}

            ctx.actions.run(
                # files also needs to be passed into argument for debugging purpose, in order to display a nice error message.
                inputs = depset([data_file], transitive = [files]),
                outputs = outputs.values(),
                progress_message = "Generating {} composition for {}".format(data["display"], ctx.label),
                arguments = _make_bdl_arguments(
                    ctx = ctx,
                    stage = "compose",
                    format = fmt,
                    search_formats = search_formats,
                    targets = targets,
                    output = "{}/{}.composition".format(outputs.values()[0].dirname, name),
                    data = data_file,
                    args = sources,
                ),
                executable = ctx.attr._bdl.files_to_run,
            )

            deps = combined_deps["all"] + ctx.attr._deps_cc
            system_providers[fmt] = {target: data["composition"]["providers"](ctx, output, deps + ([] if target == "all" else combined_deps.get(target, []))) for target, output in outputs.items()}

    return system_providers

def _bdl_system_impl(ctx):
    # Loop through all the name/target pairs and generate the composition files.
    bdl_providers = {}
    deps = {}
    for name, target in ctx.attr.targets.items():
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
    providers = _make_composition_language_providers(
        ctx = ctx,
        name = ctx.attr.name,
        deps = ctx.attr.deps,
        target_deps = deps,
        target_bdl_providers = bdl_providers,
    )

    # JSON provider should always be present.
    json = {target: data["files"]["json"] for target, data in providers["json"].items()}

    return [_BdlSystemInfo(
        data = providers,
    ), BdlSystemJsonInfo(json = json)]

_bdl_system = rule(
    implementation = _bdl_system_impl,
    doc = """Generate a system from targets.""",
    attrs = {
        "deps": attr.label_list(
            mandatory = True,
            allow_files = True,
            doc = "List of dependencies.",
            aspects = [_aspect_bdl_providers],
        ),
        "targets": attr.string_keyed_label_dict(
            mandatory = True,
            providers = [_BdlTargetInfo],
            doc = "Targets to be included for the system definition.",
        ),
        "_bdl": attr.label(
            default = Label("//bdl"),
            cfg = "exec",
            executable = True,
        ),
    } | {("_deps_" + name): (attr.label_list(
        default = data["composition"]["deps"],
    )) for name, data in _library_extensions.items() if "composition" in data},
)

def _bdl_binary_impl(ctx):
    target_provider = ctx.attr.target[_BdlTargetInfo]
    system = ctx.attr.system[_BdlSystemInfo]
    name = ctx.label.name

    fmt = target_provider.language
    if fmt in _library_extensions:
        provider = system.data[fmt][ctx.attr.target_name]
        binary = target_provider.binary
        if binary:
            binary_file = ctx.actions.declare_file(ctx.label.name + ".binary")
            runfiles = ctx.runfiles(
                files = provider.get("files", {}).values() + ctx.files.data,
            ).merge(binary[DefaultInfo].default_runfiles)
            if "runfiles" in provider:
                runfiles = runfiles.merge(provider["runfiles"])
            ctx.actions.write(
                is_executable = True,
                output = binary_file,
                content = "{executable} {args} \"$@\"".format(
                    executable = binary[DefaultInfo].files_to_run.executable.short_path,
                    args = " ".join(["--{}='{}'".format(key, value.short_path) for key, value in provider.get("files", {}).items()]),
                ),
            )
            providers = [
                DefaultInfo(executable = binary_file, runfiles = runfiles),
            ]
            metadata = []
        elif "binary" in _library_extensions[fmt]:
            providers, metadata = _library_extensions[fmt]["binary"]["build"](ctx, name, provider)
        else:
            fail("No binary associated with target language '{}'.".format(fmt))

    else:
        fail("Unsupported target language '{}'.".format(fmt))

    return [
        BzdPackageMetadataFragmentInfo(
            manifests = metadata,
        ),
        OutputGroupInfo(metadata = metadata),
        ctx.attr.system[BdlSystemJsonInfo],
    ] + providers

def _bzd_binary_generic(is_test):
    return rule(
        implementation = _bdl_binary_impl,
        doc = """Create a binary from a system rule.""",
        attrs = {
            "data": attr.label_list(
                allow_files = True,
                doc = "Files to be added to the runfiles.",
            ),
            "platform": attr.label(
                default = None,
                doc = "The platform used for the transition of this rule.",
            ),
            "system": attr.label(
                mandatory = True,
                doc = "The system rule associated with this target.",
                providers = [_BdlSystemInfo, BdlSystemJsonInfo],
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
                default = Label("@bazel_tools//tools/allowlists/function_transition_allowlist"),
            ),
            "_debug": attr.label(
                default = Label("@@//tools/bazel_build/settings/debug"),
            ),
            "_executor": attr.label(
                default = Label("@@//tools/bazel_build/settings/executor"),
            ),
            "_map_analyzer_script": attr.label(
                executable = True,
                cfg = "exec",
                default = Label("@bzd_toolchain_cc//binary/map_analyzer"),
            ),
        } | {("_metadata_" + name): (attr.label_list(
            default = data["binary"]["metadata"],
        )) for name, data in _library_extensions.items() if "binary" in data},
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

_BDL_SYSTEM_GENERIC_ATTR = {
    "deps": attr.label_list(
        doc = "Dependencies for the rule.",
    ),
    "platform": None,
    "system": None,
    "target": None,
    "target_name": None,
    "targets": attr.string_keyed_label_dict(
        doc = "A dictionary with name and target corresponding to the binaries for this system.",
        configurable = False,
    ),
    "testonly": attr.bool(
        doc = "Set testonly attribute.",
        configurable = False,
    ),
}

def _bdl_system_entry_point_impl(ctx):
    # Use the first target as the default target for this deployment.
    # It is guaranteed to have at least one target with the mandatory attribute.
    default_target = None
    for _, target in ctx.attr.targets.items():
        default_target = target
        break

    # Create an alias.
    executable = ctx.actions.declare_file(ctx.label.name)
    ctx.actions.symlink(
        output = executable,
        target_file = default_target[DefaultInfo].files_to_run.executable,
    )

    return [
        DefaultInfo(
            executable = executable,
            runfiles = default_target[DefaultInfo].default_runfiles,
            files = depset(transitive = [target[DefaultInfo].files for target in ctx.attr.targets.values()]),
        ),
        default_target[BdlSystemJsonInfo],
    ]

_bdl_system_entry_point = rule(
    implementation = _bdl_system_entry_point_impl,
    doc = """Entry point of a system rule.""",
    attrs = {
        "targets": attr.string_keyed_label_dict(
            mandatory = True,
            doc = "A dictionary with name and executable targets corresponding to the binaries for this system.",
            cfg = "target",
            providers = [BdlSystemJsonInfo],
        ),
    },
    executable = True,
    provides = [BdlSystemJsonInfo, DefaultInfo],
)

def _target_to_platform(target):
    """Convert a target label into its related platform label."""
    if Label(target).name.endswith("auto"):
        return None
    return str(Label(target)) + ".platform"

def _bdl_system_macro_impl(name, targets, testonly, deps, visibility, tags = None, target_compatible_with = None, **kwargs):
    _bdl_system(
        name = "{}.system".format(name),
        targets = targets,
        testonly = testonly,
        deps = deps,
        tags = ["manual"],
    )

    for target_name, target in targets.items():
        _bdl_binary(
            name = "{}.{}".format(name, target_name),
            platform = _target_to_platform(target),
            target = target,
            target_name = target_name,
            testonly = testonly,
            system = "{}.system".format(name),
            visibility = visibility,
            tags = tags,
            target_compatible_with = target_compatible_with,
            **kwargs
        )

    _bdl_system_entry_point(
        name = name,
        targets = {target_name: "{}.{}".format(name, target_name) for target_name, target in targets.items()},
        visibility = visibility,
        tags = tags,
        target_compatible_with = target_compatible_with,
    )

bdl_system = macro(
    implementation = _bdl_system_macro_impl,
    inherit_attrs = _bdl_binary,
    attrs = _BDL_SYSTEM_GENERIC_ATTR,
)

def _bdl_system_test_macro_impl(name, targets, testonly, deps, visibility, **kwargs):
    _bdl_system(
        name = "{}.system".format(name),
        targets = targets,
        testonly = testonly,
        deps = deps,
        tags = ["manual"],
    )

    for target_name, target in targets.items():
        _bdl_test(
            name = "{}.{}".format(name, target_name),
            platform = _target_to_platform(target),
            target = target,
            target_name = target_name,
            testonly = testonly,
            system = "{}.system".format(name),
            visibility = visibility,
            **kwargs
        )

    native.alias(
        name = name,
        actual = "{}.{}".format(name, targets.keys()[0]),
        visibility = visibility,
    )

bdl_system_test = macro(
    implementation = _bdl_system_test_macro_impl,
    inherit_attrs = _bdl_test,
    attrs = _BDL_SYSTEM_GENERIC_ATTR,
)

def _bdl_target_impl(ctx):
    return _BdlTargetInfo(
        composition = ctx.files.composition,
        deps = ctx.attr.deps,
        language = ctx.attr.language,
        binary = ctx.attr.binary,
    )

_bdl_target = rule(
    implementation = _bdl_target_impl,
    doc = """Target definition for the bzd framework.""",
    attrs = {
        "binary": attr.label(
            cfg = "exec",
            executable = True,
            doc = "Executable for the binary target.",
        ),
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
            doc = "Language associated with this target.",
            values = _library_extensions.keys(),
            mandatory = True,
        ),
    },
    provides = [_BdlTargetInfo],
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

def bdl_application_factory(implementation):
    def _bdl_application_factory_impl(ctx):
        bdl = ctx.actions.declare_file(ctx.label.name + ".bdl")
        namespace = ".".join(ctx.label.package.split("/") + [ctx.label.name])
        providers = implementation(ctx, ctx.attr.target, bdl, namespace)

        bdl_provider, _ = _precompile_bdl(ctx, srcs = [bdl], deps = [])

        return [bdl_provider] + providers

    return rule(
        implementation = _bdl_application_factory_impl,
        doc = "Wrapper for an application and add bdl information.",
        attrs = {
            "target": attr.label(
                mandatory = True,
                doc = "The target associated with this application.",
            ),
            "_bdl": attr.label(
                default = Label("//bdl"),
                cfg = "exec",
                executable = True,
            ),
        },
    )
