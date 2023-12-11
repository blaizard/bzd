load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_docker", "bzd_nodejs_library")

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
    include_metadata = True,
    visibility = [
        "//deployment:__subpackages__",
    ],
    deps = {
        "//apps/dashboard/backend": "backend",
        "//apps/dashboard/frontend": "frontend",
    },
)
