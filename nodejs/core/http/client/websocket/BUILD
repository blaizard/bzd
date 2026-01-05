load("@bzd_lib//:symlink.bzl", "bzd_symlink")
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_symlink(
    name = "symlink",
    input = select({
        "@bzd_rules_nodejs//:nodejs": "node.ws.mjs",
        "@bzd_rules_nodejs//:nodejs_web": "browser.websocket.mjs",
    }),
    output = "backend.mjs",
)

bzd_nodejs_library(
    name = "backend",
    srcs = [
        "backend.mjs",
    ],
    packages = select({
        "@bzd_rules_nodejs//:nodejs": [
            "@nodejs_deps//:ws",
        ],
        "@bzd_rules_nodejs//:nodejs_web": [],
    }),
    visibility = ["//nodejs/core/http:__pkg__"],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
    ],
)
