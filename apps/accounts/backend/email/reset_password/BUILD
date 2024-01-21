load("@bzd_lib//embed:defs.bzl", "bzd_embed")
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "reset_password",
    srcs = [
        "reset_password.mjs",
        ":reset_password_html",
        ":reset_password_txt",
    ],
    visibility = [
        "//apps/accounts/backend/email:__pkg__",
    ],
    deps = [
        "//nodejs/core:template",
    ],
)

bzd_embed(
    name = "reset_password_html",
    format = "mjs-text",
    input = "reset_password.html",
    output = "reset_password_html.mjs",
)

bzd_embed(
    name = "reset_password_txt",
    format = "mjs-text",
    input = "reset_password.txt",
    output = "reset_password_txt.mjs",
)
