load("@bzd_lib//embed:defs.bzl", "bzd_embed")
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "welcome",
    srcs = [
        "welcome.mjs",
        ":welcome_html",
        ":welcome_txt",
    ],
    visibility = [
        "//apps/accounts/backend/email:__pkg__",
    ],
    deps = [
        "//nodejs/core:template",
    ],
)

bzd_embed(
    name = "welcome_html",
    format = "mjs-text",
    input = "welcome.html",
    output = "welcome_html.mjs",
)

bzd_embed(
    name = "welcome_txt",
    format = "mjs-text",
    input = "welcome.txt",
    output = "welcome_txt.mjs",
)
