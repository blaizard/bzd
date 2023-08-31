"""Push an image remotely."""

load("@bzd_lib//lib:attrs.bzl", "ATTRS_COMMON_BUILD_RULES", "attrs_assert_any_of")
load("@rules_oci//oci:defs.bzl", "oci_push")

def bzd_docker_push(name, image, repository, remote_tags, **kwargs):
    """Pull a docker image locally.

    Args:
        name: The name of the bazel target.
        image: The image name.
        repository: Repository URL where the image will be pushed.
        remote_tags: Tags to be pushed remotely.
        **kwargs: Extra arguments common to all build rules.
    """

    attrs_assert_any_of(kwargs, ATTRS_COMMON_BUILD_RULES)

    oci_push(
        name = name,
        image = image,
        repository = repository,
        remote_tags = remote_tags,
        **kwargs
    )
