"""BDL target platform rules."""

visibility(["//..."])

def bdl_target_platform(name, target, platform, visibility = None, tags = None):
    # This is the main target rule to this target. The relation between this
    # and the platform are through their name.
    native.alias(
        name = name,
        actual = target,
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
