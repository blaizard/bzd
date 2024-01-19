load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "make_from_config",
    srcs = [
        "make_from_config.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":nodemailer",
        ":sendgrid",
        ":stub",
        "//nodejs/core:validation",
    ],
)

bzd_nodejs_library(
    name = "nodemailer",
    srcs = [
        "email.mjs",
        "nodemailer.mjs",
    ],
    packages = {
        "nodemailer": "",
    },
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:log",
    ],
)

bzd_nodejs_library(
    name = "sendgrid",
    srcs = [
        "email.mjs",
        "sendgrid.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core/http:client",
    ],
)

bzd_nodejs_library(
    name = "stub",
    srcs = [
        "email.mjs",
        "stub.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:log",
    ],
)
