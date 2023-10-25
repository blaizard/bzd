load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_binary")

bzd_nodejs_binary(
    name = "example",
    srcs = [
        "app.mjs",
    ],
    main = "app.mjs",
    packages = {
        "commander": "",
    },
    deps = [
        "//nodejs/core/http:server",
    ],
)
