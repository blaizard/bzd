load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_oci")

exports_files(
    [
        "api.v1.json",
    ],
    visibility = [
        "//apps/dashboard/backend:__pkg__",
        "//apps/dashboard/frontend:__pkg__",
    ],
)

bzd_nodejs_oci(
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
    workdir = "{backend}",
    deps = {
        "//apps/dashboard/backend": "backend",
        "//apps/dashboard/frontend": "frontend",
    },
)
