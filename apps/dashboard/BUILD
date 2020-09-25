load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_docker", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "api",
    srcs = [
        "api.v1.json",
    ],
    visibility = [
        "//apps/dashboard/backend:__pkg__",
        "//apps/dashboard/frontend:__pkg__",
    ],
)

bzd_nodejs_docker(
    name = "dashboard",
    cmd = [
        "{backend}/apps/dashboard/backend/app.mjs",
        "--static",
        "{frontend}",
    ],
    deploy = {
        "push": "docker.blaizard.com/apps/dashboard",
    },
    include_metadata = True,
    deps = {
        "//apps/dashboard/frontend": "frontend",
        "//apps/dashboard/backend": "backend",
    },
)
