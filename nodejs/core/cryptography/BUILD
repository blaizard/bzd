load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "bcrypt",
    srcs = [
        "bcrypt.mjs",
    ],
    packages = [
        "@nodejs_deps//:bcryptjs",
    ],
    visibility = ["//visibility:public"],
)
