"""Create a tarball from an image."""

load("@bzd_lib//lib:attrs.bzl", "ATTRS_COMMON_BUILD_RULES", "attrs_assert_any_of")
load("@rules_oci//oci:defs.bzl", "oci_tarball")

def bzd_docker_tarball(name, image, repository, remote_tags, **kwargs):
    """Create a docker image tarball locally.

    Args:
        name: The name of the bazel target.
        image: The image name.
        repository: Repository of the image.
        remote_tags: Tags to used.
        **kwargs: Extra arguments common to all build rules.
    """

    attrs_assert_any_of(kwargs, ATTRS_COMMON_BUILD_RULES)

    oci_tarball(
        name = name,
        image = image,
        repo_tags = ["{}:{}".format(repository, tag) for tag in remote_tags],
        **kwargs
    )
