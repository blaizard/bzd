"""BDL target rules."""

load("//private:common.bzl", "aspect_bdl_providers", "library_extensions")
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
        language = parent.language
        binary = parent.binary

    else:
        composition = ctx.files.composition
        deps = ctx.attr.deps
        language = ctx.attr.language
        binary = ctx.attr.binary

    if not composition:
        fail("The target {} is missing composition files".format(ctx.label))

    return BdlTargetInfo(
        composition = composition,
        deps = deps,
        language = language,
        binary = binary,
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
            allow_files = [".bdl"],
            doc = "List of composition bdl source files for this target.",
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
    },
    provides = [BdlTargetInfo],
)

def _bdl_target_macro_impl(name, visibility, parent, platform, **kwargs):
    if name.endswith("auto"):
        if platform != None:
            fail("bdl_target which names end with 'auto' cannot have platform defined.")

    _bdl_target(
        name = name,
        visibility = visibility,
        parent = parent,
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
