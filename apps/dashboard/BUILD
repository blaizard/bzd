load("@io_bazel_rules_docker//container:container.bzl", "container_image", "container_push")
load("//tools/bazel.build/rules:package.bzl", "bzd_package")
load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "api",
    srcs = [
        "api.v1.json",
    ],
    alias = "dashboard",
    visibility = [
        "//apps/dashboard/backend:__pkg__",
        "//apps/dashboard/frontend:__pkg__",
    ],
)

bzd_nodejs_library(
    name = "plugins",
    srcs = glob([
        "plugins/**/*",
    ]),
    alias = "dashboard",
    visibility = [
        "//apps/dashboard:__subpackages__",
    ],
    deps = [
        "//nodejs/vue/components:graph",
    ],
)

bzd_package(
    name = "package",
    tags = ["nodejs"],
    deps = {
        "//apps/dashboard/frontend": "frontend",
        "//apps/dashboard/backend": "backend",
    },
)

container_image(
    name = "dashboard",
    base = "@docker_nodejs//image",
    cmd = [
        "node",
        "--experimental-json-modules",
        "/bzd/bin/backend/apps/dashboard/backend/app.js",
        "--static",
        "/bzd/bin/frontend",
    ],
    env = {
        "NODE_ENV": "production", 
    },
    directory = "/bzd/bin",
    tags = ["nodejs"],
    tars = [
        ":package",
    ],
)

container_push(
    name = "push",
    format = "Docker",
    image = ":dashboard",
    registry = "docker.blaizard.com",
    repository = "apps/dashboard",
    tag = "latest",
    tags = [
        "docker",
    ],
)
