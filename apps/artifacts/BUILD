load("@io_bazel_rules_docker//container:container.bzl", "container_image")
load("//tools/bazel_build/rules:package.bzl", "bzd_package")
load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "api",
    srcs = [
        "api.v1.json",
    ],
    visibility = [
        "//apps/artifacts/backend:__pkg__",
        "//apps/artifacts/frontend:__pkg__",
    ],
)

bzd_package(
    name = "package",
    tags = ["nodejs"],
    deps = {
        "//apps/artifacts/frontend": "frontend",
        "//apps/artifacts/backend": "backend",
    },
)

container_image(
    name = "artifacts",
    base = "@docker_nodejs//image",
    cmd = [
        "node",
        "--experimental-json-modules",
        "/bzd/bin/backend/apps/artifacts/backend/app.mjs",
        "--static",
        "/bzd/bin/frontend",
    ],
    directory = "/bzd/bin",
    env = {
        "NODE_ENV": "production",
    },
    tags = ["nodejs"],
    tars = [
        ":package",
    ],
)

container_push(
    name = "push",
    format = "Docker",
    image = ":artifacts",
    registry = "docker.blaizard.com",
    repository = "apps/artifacts",
    tag = "latest",
    tags = [
        "docker",
    ],
)
