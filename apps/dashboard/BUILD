load("@io_bazel_rules_docker//container:container.bzl", "container_image")
load("//tools/bazel.build/rules:package.bzl", "bzd_package")
load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "api",
    srcs = [
        "api.v1.json"
    ],
    alias = "dashboard",
    visibility = [
        "//apps/dashboard/frontend:__pkg__",
        "//apps/dashboard/backend:__pkg__",
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
    base = "@apline_node//image",
    cmd = [
        "node",
        "/bzd/bin/backend/apps/dashboard/backend/app.js",
        "--static",
        "/bzd/bin/frontend",
    ],
    directory = "/bzd/bin",
    tags = ["nodejs"],
    tars = [
        ":package",
    ],
)
