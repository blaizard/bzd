"""BDL target platform rules."""

load("//private:common.bzl", "transition_platform")
load("//private:providers.bzl", "BdlTargetInfo")

visibility(["//..."])

def _bdl_target_platform_impl(ctx):
    return ctx.attr.target[BdlTargetInfo]

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
            providers = [BdlTargetInfo],
            doc = "The target associated with this target platform.",
        ),
        "_allowlist_function_transition": attr.label(
            default = "@bazel_tools//tools/allowlists/function_transition_allowlist",
        ),
    },
    cfg = transition_platform,
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
