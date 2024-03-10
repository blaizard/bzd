load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_docker")

exports_files(
    [
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
    include_metadata = True,
    visibility = [
        "//deployment:__subpackages__",
    ],
    deps = {
        "//apps/artifacts/backend": "backend",
        "//apps/artifacts/frontend": "frontend",
    },
)
