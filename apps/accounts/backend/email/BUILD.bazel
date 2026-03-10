load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "email",
    srcs = [
        "manager.mjs",
    ],
    visibility = [
        "//apps/accounts/backend:__subpackages__",
    ],
    deps = [
        "//apps/accounts/backend/email/reset_password",
        "//apps/accounts/backend/email/welcome",
        "//nodejs/core:exception",
        "//nodejs/core:log",
    ],
)
