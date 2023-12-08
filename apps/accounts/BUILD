load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_docker")

exports_files([
    "api.v1.json",
    "config.json",
])

bzd_nodejs_docker(
    name = "accounts",
    cmd = [
        "{backend}/apps/accounts/backend/app.mjs",
        "--static",
        "{frontend}",
    ],
    deploy = {
        "push": "docker.blaizard.com/apps/accounts",
    },
    include_metadata = True,
    deps = {
        "//apps/accounts/backend": "backend",
        "//apps/accounts/frontend": "frontend",
    },
)
