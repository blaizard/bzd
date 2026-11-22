"""BDL target rules."""

load("@bzd_lib//:defs.bzl", "BzdRunnerOptionsInfo")
load("//private:common.bzl", "aspect_bdl_providers", "library_extensions", "transition_platform")
load("//private:providers.bzl", "BdlTargetInfo")

visibility(["//..."])

def _bdl_target_impl(ctx):
    if ctx.attr.parent:
        parent = ctx.attr.parent[BdlTargetInfo]

        if ctx.attr.language:
            fail("Language can only be set by top level targets, not {}.".format(ctx.label))
        if ctx.attr.binary:
            fail("Binary can only be set by top level targets, not {}.".format(ctx.label))

        composition = parent.composition + ctx.files.composition
        deps = parent.deps + ctx.attr.deps
        data = ctx.attr.data | parent.data
        language = parent.language
        binary = parent.binary
        presets = ctx.files.presets + parent.presets

    else:
        composition = ctx.files.composition
        deps = ctx.attr.deps
        data = ctx.attr.data
        language = ctx.attr.language
        binary = ctx.attr.binary
        presets = ctx.files.presets

    if not composition:
        fail("The target {} is missing composition files".format(ctx.label))

    runner_options = ctx.attr.runner_options or ctx.attr.parent
    if runner_options != None:
        runner_options = runner_options[BzdRunnerOptionsInfo]
    else:
        runner_options = BzdRunnerOptionsInfo(files = {}, values = {})

    return [
        BdlTargetInfo(
            composition = composition,
            deps = deps,
            data = data,
            language = language,
            binary = binary,
            presets = presets,
        ),
        runner_options,
    ]

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
            allow_files = [".bdl"],
            doc = "List of composition bdl source files for this target.",
        ),
        "data": attr.label_list_dict(
            doc = "Additional files and/or bazel targets to be associated with this target.",
            allow_files = True,
        ),
        "deps": attr.label_list(
            doc = "List of dependencies.",
            aspects = [aspect_bdl_providers],
        ),
        "language": attr.string(
            doc = "Language associated with this target.",
            values = library_extensions.keys(),
        ),
        "parent": attr.label(
            doc = "The parent target if it extends an existing target.",
            providers = [BdlTargetInfo],
        ),
        "platform": attr.label(
            doc = "The platform used for the transition of this target.",
        ),
        "presets": attr.label_list(
            allow_files = [".json"],
            doc = "JSON preset files referenced by `preset NAME from \"PATH\";` statements in the composition files.",
        ),
        "runner_options": attr.label(
            providers = [BzdRunnerOptionsInfo],
            doc = "Options passed to the runner of the binary, inherited from the parent if not set.",
        ),
        "_allowlist_function_transition": attr.label(
            default = "@bazel_tools//tools/allowlists/function_transition_allowlist",
        ),
    },
    cfg = transition_platform,
    provides = [BdlTargetInfo],
)

def _bdl_target_macro_impl(name, visibility, parent, platform, **kwargs):
    if name.endswith("auto"):
        if platform != None:
            fail("bdl_target which names end with 'auto' cannot have platform defined.")

    # Reference the `<name>.platform` alias for the transition, the same label used by
    # `bdl_system`, so that both resolve to a single configuration.
    _bdl_target(
        name = name,
        visibility = visibility,
        parent = parent,
        platform = ":{}.platform".format(name) if platform != None else None,
        **kwargs
    )

    native.alias(
        name = "{}.platform".format(name),
        actual = platform if platform else ("{}.platform".format(Label(parent)) if parent else "@platforms//host"),
        visibility = visibility,
    )

bdl_target = macro(
    inherit_attrs = _bdl_target,
    implementation = _bdl_target_macro_impl,
    attrs = {
        "parent": attr.label(
            doc = "The parent target if it extends an existing target.",
            configurable = False,
        ),
        "platform": attr.label(
            doc = "The platform used for the transition of this target.",
            configurable = False,
        ),
    },
)
