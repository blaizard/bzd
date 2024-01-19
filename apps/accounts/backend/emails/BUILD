load("@bzd_lib//embed:defs.bzl", "bzd_embed")
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "emails",
    srcs = [
        "manager.mjs",
        ":reset_password",
    ],
    visibility = [
        "//apps/accounts/backend:__subpackages__",
    ],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core:template",
        "//nodejs/email:nodemailer",
    ],
)

bzd_embed(
    name = "reset_password",
    format = "mjs-text",
    input = "reset_password.html",
    output = "reset_password.mjs",
)
