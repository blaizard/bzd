"""BDL target rules."""

load("//private:common.bzl", "aspect_bdl_providers", "library_extensions")
load("//private:providers.bzl", "BdlTargetInfo")

visibility(["//..."])

def _bdl_target_impl(ctx):
    return BdlTargetInfo(
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
            aspects = [aspect_bdl_providers],
        ),
        "language": attr.string(
            doc = "Language associated with this target.",
            values = library_extensions.keys(),
            mandatory = True,
        ),
    },
    provides = [BdlTargetInfo],
)

def bdl_target(name, **kwargs):
    if not name.endswith("auto"):
        fail("The name for bdl_target must ends with 'auto', this is to tell the rule that it does not have related platform.")
    _bdl_target(
        name = name,
        **kwargs
    )
