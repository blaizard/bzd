"""BDL rule helpers."""

load("@bazel_skylib//lib:sets.bzl", "sets")
load("@bdl_extension//:extensions.bzl", "extensions")
load("//private:providers.bzl", "BdlInfo")

visibility(["//..."])

# ---- Extensions ----

library_extensions = {} | extensions

# ---- Aspects ----

def _aspect_bdl_providers_impl(target, ctx):
    """Aspects to gather all bdl dependency outputs."""

    has_deps = hasattr(ctx.rule.attr, "deps")
    if BdlInfo not in target and has_deps:
        return [
            BdlInfo(
                sources = depset(transitive = [dep[BdlInfo].sources for dep in ctx.rule.attr.deps if BdlInfo in dep]),
                files = depset(transitive = [dep[BdlInfo].files for dep in ctx.rule.attr.deps if BdlInfo in dep]),
                search_paths = sets.to_list(sets.make([d for dep in ctx.rule.attr.deps if BdlInfo in dep for d in dep[BdlInfo].search_paths])),
            ),
        ]

    return []

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

def make_bdl_arguments(stage, search_paths = None, format = None, output = None, namespace = None, data = None, targets = None, args = None):
    """Create the argument list for the `bdl` tool.

    Args:
        stage: The stage of the bdl processing.
        search_paths: List of directory paths to search for preprocessed object files (and preset files) from upstream rules.
        format: The format of the output.
        output: The output path.
        namespace: The namespace to set.
        data: The data to use.
        targets: The targets associated with this bdl processing.
        args: Extra arguments.

    Returns:
        The argument list to be passed to the bdl tool.
    """

    arguments = ["--stage", stage]
    if search_paths:
        arguments += [i for path in search_paths for i in ("--search-path", path)]
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

def precompile_bdl(ctx, srcs, deps, output_dir = None, namespace = None, presets = []):
    """Precompile a set of bdls.

    Args:
        ctx: The context used for this action.
        srcs: The set of bdls to be precompiled.
        deps: The dependencies associated with these bdls, must have a `BdlInfo`.
        output_dir: The output directory where the precompiled objects should be stored,
                    if not specified, it will be stored in the same directory as the source file.
        namespace: The namespace in which the bdls files should be compiled.
        presets: JSON preset files referenced by `preset NAME from "PATH";` statements.

    Returns:
        A tuple including a BdlInfo provider and its associated metadata
    """

    # Input files and bdls
    input_sources = depset(transitive = [dep[BdlInfo].sources for dep in deps])
    input_files = depset(srcs, transitive = [dep[BdlInfo].files for dep in deps])
    preset_files = depset(presets)

    # Collect search paths from all dependencies.
    search_paths = sets.make([d for dep in deps for d in dep[BdlInfo].search_paths])
    sets.insert(search_paths, ctx.bin_dir.path)
    search_paths = sets.to_list(search_paths)

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

    # Preprocess all input files at once, this stage is language agnostic.
    ctx.actions.run(
        inputs = depset(transitive = [input_files, preset_files]),
        outputs = [bdl["output"] for bdl in metadata],
        progress_message = "Preprocessing BDL manifest(s) {}".format(", ".join([bdl["input"].short_path for bdl in metadata])),
        arguments = make_bdl_arguments(
            stage = "preprocess",
            namespace = namespace,
            search_paths = search_paths,
            args = ["{}@{}".format(bdl["input"].path, bdl["output"].path) for bdl in metadata],
        ),
        executable = ctx.attr._bdl.files_to_run,
    )

    direct = [bdl["output"] for bdl in metadata]
    sources = depset([(bdl["input"], bdl["output"]) for bdl in metadata], transitive = [input_sources])
    files = depset([bdl["output"] for bdl in metadata], transitive = [input_files])

    return BdlInfo(sources = sources, files = files, search_paths = search_paths, direct = direct), metadata
