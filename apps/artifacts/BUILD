load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_oci")

exports_files(
    [
        "api.v1.json",
    ],
    visibility = [
        "//apps/artifacts/backend:__pkg__",
        "//apps/artifacts/frontend:__pkg__",
    ],
)

bzd_nodejs_oci(
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
    workdir = "{backend}",
    deps = {
        "//apps/artifacts/backend": "backend",
        "//apps/artifacts/frontend": "frontend",
    },
)
