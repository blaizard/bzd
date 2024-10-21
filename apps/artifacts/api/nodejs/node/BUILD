load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "node",
    srcs = [
        "node.mjs",
        "//apps/artifacts/api:config.json",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core/http:client",
        "//nodejs/db:utils",
    ],
)
