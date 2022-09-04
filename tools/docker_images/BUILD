load("//tools/docker_images:register.bzl", "IMAGES")
load("@io_bazel_rules_docker//container:container.bzl", "container_image")
load("//tools/bazel_build:multi_run.bzl", "multi_run")

# Reference docker images from within the repository.
[container_image(
    name = name,
    base = "@docker_image_{}//image".format(name),
    tags = [
        "docker-pull",
        "manual",
    ],
    visibility = ["//visibility:public"],
) for name in IMAGES.keys()]

# Run this targe to load all images into docker.
multi_run(
    name = "docker_images",
    tags = ["manual"],
    targets = [":{}".format(name) for name in IMAGES.keys()],
)
