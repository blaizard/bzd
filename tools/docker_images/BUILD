load("//tools/docker_images:register.bzl", "IMAGES")
load("//tools/bazel_build:multi_run.bzl", "multi_run")
load("//tools/bazel_build/rules:docker.bzl", "bzd_docker_load")

# Reference docker images from within the repository.
[bzd_docker_load(
    name = name,
    digest = data.digest,
    registry = data.registry,
    repository = data.repository,
    tags = ["manual"],
    visibility = ["//visibility:public"],
) for name, data in IMAGES.items()]

# Run this targe to load all images into docker.
multi_run(
    name = "docker_images",
    tags = ["manual"],
    targets = [":{}".format(name) for name in IMAGES.keys()],
)
