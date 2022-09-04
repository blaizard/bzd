load("@io_bazel_rules_docker//container:container.bzl", "container_pull")

DOCKER_TAG = "docker"

def bzd_docker_pull(**kwargs):
    """Pull a docker image."""

    container_pull(timeout = 3600, **kwargs)
