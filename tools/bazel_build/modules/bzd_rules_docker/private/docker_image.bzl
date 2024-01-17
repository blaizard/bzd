"""NodeJs docker rule."""

load("@bzd_lib//lib:attrs.bzl", "ATTRS_COMMON_BUILD_RULES", "attrs_assert_any_of")
load("@rules_oci//oci:defs.bzl", "oci_image")

def bzd_docker_image(name, base = None, cmd = [], workdir = None, env = {}, tars = [], entrypoint = [], **kwargs):
    """Build a container image.

    Args:
        name: A unique name for this target.
        base: Label to another image target to use as the base.
        cmd: Default arguments to the entrypoint of the container.
        workdir: Sets the current working directory of the entrypoint process in the container.
        env: Environment variables provisioned by default to the running container.
        tars: List of tar files to add to the image as layers.
        entrypoint: Entrypoint of the container.
        **kwargs: Extra arguments common to all build rules.
    """

    attrs_assert_any_of(kwargs, ATTRS_COMMON_BUILD_RULES)

    oci_image(
        name = name,
        base = base,
        cmd = cmd,
        workdir = workdir,
        env = env,
        tars = tars,
        entrypoint = entrypoint,
        **kwargs
    )
