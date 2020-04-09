load("@io_bazel_rules_docker//container:container.bzl", "container_image")
load("//tools/bazel.build/rules:package.bzl", "bzd_package")

bzd_package(
    name = "package",
    deps = [
        "//apps/dashboard/frontend",
        "//apps/dashboard/backend",
    ],
    tags = ["nodejs"],
)

container_image(
    name = "dashboard",
    base = "@apline_node//image",
    directory = "/data",
    tars = [
        ":package"
    ],
    cmd = ["/usr/bin/node /data/apps.dashboard.backend.backend/apps/dashboard/backend/app.js"],
    tags = ["nodejs"],
)
