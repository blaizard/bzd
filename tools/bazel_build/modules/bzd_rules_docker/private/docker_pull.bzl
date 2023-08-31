"""Pull an image locally."""

load("@bzd_lib//lib:attrs.bzl", "ATTRS_COMMON_BUILD_RULES", "attrs_assert_any_of")
load("@rules_oci//oci:pull.bzl", "oci_pull")

def bzd_docker_pull(name, image, digest, **kwargs):
    """Pull a docker image locally.

    Args:
        name: The name of the bazel target.
        image: The image name.
        digest: The digest of the image.
        **kwargs: Extra arguments common to all build rules.
    """

    attrs_assert_any_of(kwargs, ATTRS_COMMON_BUILD_RULES)

    oci_pull(
        name = name,
        image = image,
        digest = digest,
        **kwargs
    )
