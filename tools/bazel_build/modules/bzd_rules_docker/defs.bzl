"""Registry for docker images used in this repository."""

load("@bzd_rules_docker//:private/docker_image.bzl", bzd_docker_image_ = "bzd_docker_image")
load("@bzd_rules_docker//:private/docker_images_config.bzl", bzd_docker_images_config_ = "bzd_docker_images_config")
load("@bzd_rules_docker//:private/docker_pull.bzl", bzd_docker_pull_ = "bzd_docker_pull")
load("@bzd_rules_docker//:private/docker_push.bzl", bzd_docker_push_ = "bzd_docker_push")

bzd_docker_images_config = bzd_docker_images_config_
bzd_docker_pull = bzd_docker_pull_
bzd_docker_push = bzd_docker_push_
bzd_docker_image = bzd_docker_image_
