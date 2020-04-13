load("@io_bazel_rules_docker//container:container.bzl", "container_image")
load("//tools/bazel.build/rules:package.bzl", "bzd_package")

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
        "/data/backend/apps/dashboard/backend/app.js",
        "--static",
        "/data/frontend",
    ],
    directory = "/data",
    tags = ["nodejs"],
    tars = [
        ":package",
    ],
)
