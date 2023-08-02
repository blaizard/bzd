load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_docker", "bzd_nodejs_library")

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

bzd_nodejs_docker(
    name = "artifacts",
    cmd = [
        "{backend}/apps/artifacts/backend/app.mjs",
        "--static",
        "{frontend}",
    ],
    deploy = {
        "push": "docker.blaizard.com/apps/artifacts",
    },
    include_metadata = True,
    deps = {
        "//apps/artifacts/backend": "backend",
        "//apps/artifacts/frontend": "frontend",
    },
)
