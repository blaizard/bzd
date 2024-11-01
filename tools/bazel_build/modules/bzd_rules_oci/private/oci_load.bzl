"""Create a tarball from an image."""

load("@bzd_lib//lib:attrs.bzl", "ATTRS_COMMON_BUILD_RULES", "attrs_assert_any_of")
load("@rules_oci//oci:defs.bzl", "oci_load")

def bzd_oci_load(name, image, repository, remote_tags, **kwargs):
    """Load an image into docker.

    Args:
        name: The name of the bazel target.
        image: The image name.
        repository: Repository of the image.
        remote_tags: Tags to used.
        **kwargs: Extra arguments common to all build rules.
    """

    attrs_assert_any_of(kwargs, ATTRS_COMMON_BUILD_RULES)

    oci_load(
        name = name,
        image = image,
        repo_tags = ["{}:{}".format(repository, tag) for tag in remote_tags],
        **kwargs
    )
