load("//tools/bazel_build/rules:docker.bzl", "bzd_docker_load")
load("//tools/docker_images:register.bzl", "IMAGES", "docker_images_config")

# Pre-load images into docker.
[bzd_docker_load(
    name = name,
    digest = data.digest,
    registry = data.registry,
    repository = data.repository,
    tags = ["manual"],
    visibility = ["//visibility:public"],
) for name, data in IMAGES.items()]

docker_images_config(
    name = "config",
    out = "config.json",
    visibility = ["//python/bzd/utils:__pkg__"],
)
