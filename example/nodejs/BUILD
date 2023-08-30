load("@bzd_package//:defs.bzl", "bzd_package")
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_binary")

bzd_nodejs_binary(
    name = "nodejs",
    srcs = [
        "app.mjs",
    ],
    main = "app.mjs",
    packages = {
        "mustache": "",
    },
    visibility = [
        "//apps/dashboard:__pkg__",
    ],
)

bzd_package(
    name = "package",
    tags = ["nodejs"],
    deps = {
        ":nodejs": "nodejs",
    },
)
