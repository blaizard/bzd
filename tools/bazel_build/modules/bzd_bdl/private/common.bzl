"""BDL rule helpers."""

load("@bazel_skylib//lib:sets.bzl", "sets")
load("@bdl_extension//:extensions.bzl", "extensions")
load("//private:providers.bzl", "BdlInfo", "BdlTagInfo")
load("//private/extensions:json.bzl", extension_json = "extension")

visibility(["//..."])

# ---- Extensions ----

library_extensions = {} | extension_json | extensions

# ---- Private Providers ----

_BdlCompositionInfo = provider(
    doc = "Provider to gather language specific data for composition.",
    fields = {
        "data": "Data specific keyed by extension.",
    },
)

# ---- Aspects ----

def _aspect_bdl_providers_impl(target, ctx):
    """Aspects to gather all bdl dependency outputs."""

    # Are considered composition public headers when the target is not a BDL library but has a bdl library as a direct dependency.
    # Then it means it relies on a BDL interface.
    has_deps = hasattr(ctx.rule.attr, "deps")
    is_direct = (BdlTagInfo not in target) and has_deps and (any([dep for dep in ctx.rule.attr.deps if BdlTagInfo in dep]))
    transitive_composition = [dep[_BdlCompositionInfo] for dep in ctx.rule.attr.deps if _BdlCompositionInfo in dep] if has_deps else []
    provider_data = {fmt: {} for fmt in library_extensions.keys()}
    for fmt, data in library_extensions.items():
        if "aspect_files" in data:
            for key, aspect in data["aspect_files"].items():
                data = aspect(target) if is_direct else []
                provider_data[fmt][key] = depset(data, transitive = [t.data[fmt].get(key, depset()) for t in transitive_composition])

    if BdlInfo not in target and has_deps:
        return [
            BdlInfo(
                sources = depset(transitive = [dep[BdlInfo].sources for dep in ctx.rule.attr.deps if BdlInfo in dep]),
                files = depset(transitive = [dep[BdlInfo].files for dep in ctx.rule.attr.deps if BdlInfo in dep]),
                search_formats = sets.to_list(sets.make([d for dep in ctx.rule.attr.deps if BdlInfo in dep for d in dep[BdlInfo].search_formats])),
            ),
            _BdlCompositionInfo(data = provider_data),
        ]

    return [_BdlCompositionInfo(data = provider_data)]

aspect_bdl_providers = aspect(
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

transition_platform = transition(
    implementation = _transition_platform_impl,
    inputs = [],
    outputs = [
        "//command_line_option:platforms",
    ],
)

# ---- Helpers functions ----

def _get_preprocessed_format(ctx):
    return "{}/{{}}.o".format(ctx.bin_dir.path)

def make_bdl_arguments(ctx, stage, search_formats = None, format = None, output = None, namespace = None, data = None, targets = None, args = None):
    """Create the argument list for the `bdl` tool.

    Args:
        ctx: The context of the rule.
        stage: The stage of the bdl processing.
        search_formats: The formatting of the search.
        format: The format of the output.
        output: The output path.
        namespace: The namespace to set.
        data: The data to use.
        targets: The targets associated with this bdl processing.
        args: Extra arguments.

    Returns:
        The argument list to be passed to the bdl tool.
    """

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

def precompile_bdl(ctx, srcs, deps, output_dir = None, namespace = None):
    """Precompile a set of bdls.

    Args:
        ctx: The context used for this action.
        srcs: The set of bdls to be precompiled.
        deps: The dependencies associated with these bdls, must have a `BdlInfo`.
        output_dir: The output directory where the precompiled objects should be stored,
                    if not specified, it will be stored in the same directory as the source file.
        namespace: The namespace in which the bdls files should be compiled.

    Returns:
        A tuple including a BdlInfo provider and its associated metadata
    """

    # Input files and bdls
    input_sources = depset(transitive = [dep[BdlInfo].sources for dep in deps])
    input_files = depset(srcs, transitive = [dep[BdlInfo].files for dep in deps])
    search_formats = sets.make([d for dep in deps for d in dep[BdlInfo].search_formats])

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
        arguments = make_bdl_arguments(
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

    return BdlInfo(sources = sources, files = files, search_formats = search_formats), metadata

def make_composition_language_providers(ctx, name, deps, target_deps = None, target_bdl_providers = None):
    """Compose the system.

    Args:
        ctx: The context of the action running the composition.
        name: The name of the generated output.
        deps: The dependencies associated with this action, it shall contain BdlInfo and language specific providers
              such as CcInfo, etc.
        target_deps: Dictionary keyed by target name of the dependencies.
        target_bdl_providers: Dictionary keyed by target of extra BdlInfo to be added.

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

    # Contains all BDL providers following this schema: List[Pair[BdlInfo, Optional[target]]]
    bdl_providers = [(dep[BdlInfo], None) for dep in deps if BdlInfo in dep] + [(provider, name) for name, provider in target_bdl_providers.items()]

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
    for fmt, data in library_extensions.items():
        if "composition" in data:
            language_specific_data[fmt] = data["composition"]["data"](deps_to_info(fmt, deps), {target: deps_to_info(fmt, deps) for target, deps in target_deps.items()})

    # Write the language specific data file.
    data_file = ctx.actions.declare_file("{}.data.json".format(name))
    ctx.actions.write(
        output = data_file,
        content = json.encode(language_specific_data),
    )

    system_providers = {}
    for fmt, data in library_extensions.items():
        if "composition" in data:
            # Generate the expected output files.
            outputs = {target: ctx.actions.declare_file(data["composition"]["output"].format(name = name, target = target)) for target in (targets if targets else ["all"])}

            ctx.actions.run(
                # files also needs to be passed into argument for debugging purpose, in order to display a nice error message.
                inputs = depset([data_file], transitive = [files]),
                outputs = outputs.values(),
                progress_message = "Generating {} composition for {}".format(data["display"], ctx.label),
                arguments = make_bdl_arguments(
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
