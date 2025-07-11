load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_oci")

exports_files(
    [
        "api.v1.json",
        "jobs.json",
    ],
    visibility = [
        "//apps/job_executor/backend:__pkg__",
        "//apps/job_executor/frontend:__pkg__",
    ],
)

bzd_nodejs_oci(
    name = "job_executor",
    cmd = [
        "{backend}/apps/job_executor/backend/app.mjs",
        "--static",
        "{frontend}",
    ],
    include_metadata = True,
    visibility = [
        "//deployment:__subpackages__",
    ],
    workdir = "{backend}",
    deps = {
        "//apps/job_executor/backend": "backend",
        "//apps/job_executor/frontend": "frontend",
    },
)
