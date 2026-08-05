"""BDL system rules."""

load("@bdl_extension//:extensions.bzl", "extensions")
load("//private:common.bzl", "aspect_bdl_providers", "precompile_bdl", "transition_platform")
load("//private:providers.bzl", "BdlCompositionInfo", "BdlInfo", "BdlSystemJsonInfo", "BdlTargetInfo")

visibility(["//..."])

def _bdl_composition_impl(ctx):
    # Loop through all the name/target pairs and generate the composition files.
    bdl_providers = {}
    deps = {}

    # Generate the composition per format and per target.
    for name, target in ctx.attr.targets.items():
        target_provider = target[BdlTargetInfo]
        target_name = "{}.{}".format(ctx.label.name, name)

        # Generate the target-specific composition by injecting the new namespace.
        bdl_provider, _ = precompile_bdl(
            ctx = ctx,
            srcs = target_provider.composition,
            deps = target_provider.deps,
            output_dir = target_name,
            namespace = name,
            presets = [],
        )
        bdl_providers[name] = bdl_provider
        deps[name] = target_provider.deps

    return [BdlCompositionInfo(
        deps = deps,
        bdls = bdl_providers.values() + [dep[BdlInfo] for dep in ctx.attr.deps if BdlInfo in dep],
    )]

_bdl_composition = rule(
    implementation = _bdl_composition_impl,
    doc = """Generate a system from targets.""",
    attrs = {
        "deps": attr.label_list(
            mandatory = True,
            allow_files = True,
            doc = "List of dependencies.",
            aspects = [aspect_bdl_providers],
        ),
        "targets": attr.string_keyed_label_dict(
            mandatory = True,
            providers = [BdlTargetInfo],
            doc = "Targets to be included for the system definition.",
        ),
        "_bdl": attr.label(
            default = Label("//bdl"),
            cfg = "exec",
            executable = True,
        ),
    },
)

def _bdl_binary_impl(ctx):
    target = ctx.attr.target[BdlTargetInfo]
    composition = ctx.attr.compositions.get(target.language)
    if target.language and not composition:
        fail("Composition for language '{}' was requested, but it is not implemented.".format(target.language))
    composition_default_info = composition[DefaultInfo] if composition else DefaultInfo(files = depset([]))

    if target.binary:
        files = composition_default_info.files.to_list()
        runfiles = ctx.runfiles(
            files = ctx.files.data + files,
        ).merge(target.binary[DefaultInfo].default_runfiles)
        if composition_default_info.default_runfiles:
            runfiles = runfiles.merge(composition_default_info.default_runfiles)
        ctx.actions.write(
            output = ctx.outputs.executable,
            content = "{executable} {args} \"$@\"".format(
                executable = target.binary[DefaultInfo].files_to_run.executable.short_path,
                args = " ".join(["--{}='{}'".format(target.language, f.short_path) for f in files]),
            ),
            is_executable = True,
        )
        return DefaultInfo(
            executable = ctx.outputs.executable,
            runfiles = runfiles,
            files = depset(files),
        )

    elif composition_default_info.default_runfiles:
        ctx.actions.symlink(
            output = ctx.outputs.executable,
            target_file = composition_default_info.files_to_run.executable,
            is_executable = True,
        )
        return DefaultInfo(
            executable = ctx.outputs.executable,
            runfiles = composition_default_info.default_runfiles,
        )

    else:
        fail("No binary associated with target '{}' -> '{}'.".format(ctx.label, ctx.attr.target_name))

_bdl_binary = rule(
    implementation = _bdl_binary_impl,
    doc = """Create a binary from a system rule.""",
    attrs = {
        "compositions": attr.string_keyed_label_dict(
            mandatory = True,
            doc = "The composition per language.",
        ),
        "data": attr.label_list(
            allow_files = True,
            doc = "Files to be added to the runfiles.",
        ),
        "platform": attr.label(
            default = None,
            doc = "The platform used for the transition of this rule.",
        ),
        "target": attr.label(
            mandatory = True,
            doc = "The target label for this binary.",
            providers = [BdlTargetInfo],
        ),
        "target_name": attr.string(
            mandatory = True,
            doc = "The name of the target.",
        ),
        "_allowlist_function_transition": attr.label(
            default = Label("@bazel_tools//tools/allowlists/function_transition_allowlist"),
        ),
    },
    cfg = transition_platform,
    executable = True,
)

def _bdl_system_impl(ctx):
    return [
        BdlSystemJsonInfo(
            json = {target_name: target[DefaultInfo].files.to_list()[0] for target_name, target in ctx.attr.json_compositions.items()},
        ),
    ]

_bdl_system = rule(
    implementation = _bdl_system_impl,
    doc = """Create a system.""",
    attrs = {
        "json_compositions": attr.string_keyed_label_dict(
            mandatory = True,
            doc = "The json composition for each target.",
        ),
    },
    provides = [BdlSystemJsonInfo],
)

def _target_to_platform(target):
    """Convert a target label into its related platform label."""
    if Label(target).name.endswith("auto"):
        return None
    return "{}.platform".format(str(Label(target)))

def _bdl_system_macro_impl(name, visibility, targets, testonly, deps, data, **kwargs):
    _bdl_composition(
        name = "{}.composition".format(name),
        targets = targets,
        visibility = visibility,
        deps = deps,
        testonly = testonly,
        tags = ["manual"],
    )

    for target_name, target in targets.items():
        compositions = {}
        for fmt, extension in extensions.items():
            if "composition" in extension:
                compositions[fmt] = "{}.{}.{}.composition".format(name, target_name, fmt)
                extension["composition"]["generator"](
                    name = compositions[fmt],
                    target_name = target_name,
                    target = target,
                    composition = "{}.composition".format(name),
                    deps = deps,
                    testonly = testonly,
                    tags = ["manual"],
                )
        _bdl_binary(
            name = "{}.{}".format(name, target_name),
            platform = _target_to_platform(target),
            target_name = target_name,
            target = target,
            testonly = testonly,
            data = data,
            compositions = compositions,
            visibility = visibility,
        )

    _bdl_system(
        name = name,
        json_compositions = {target_name: ":{}.{}.json.composition".format(name, target_name) for target_name in targets.keys()},
        testonly = testonly,
        visibility = visibility,
        **kwargs
    )

bdl_system = macro(
    implementation = _bdl_system_macro_impl,
    inherit_attrs = _bdl_system,
    attrs = {
        "data": attr.label_list(
            doc = "Data for the rule.",
        ),
        "deps": attr.label_list(
            doc = "Dependencies for the rule.",
        ),
        "json_compositions": None,
        "targets": attr.string_keyed_label_dict(
            doc = "A dictionary with name and target corresponding to the binaries for this system.",
            configurable = False,
        ),
    },
)
