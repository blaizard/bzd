load("@bzd_lib//config:defs.bzl", "bzd_config_default")
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_oci")

exports_files([
    "api.v1.json",
])

bzd_config_default(
    name = "config",
    srcs = [
        "config.default.json",
    ],
    visibility = [
        ":__subpackages__",
        "//deployment:__subpackages__",
    ],
)

bzd_nodejs_oci(
    name = "accounts",
    cmd = [
        "{backend}/apps/accounts/backend/app.mjs",
        "--static",
        "{frontend}",
    ],
    include_metadata = True,
    visibility = [
        "//deployment:__subpackages__",
    ],
    deps = {
        "//apps/accounts/backend": "backend",
        "//apps/accounts/frontend": "frontend",
    },
)
