load("@bzd_lib//:symlink.bzl", "bzd_symlink")

bzd_symlink(
    name = "backend",
    input = select({
        "@bzd_rules_nodejs//:nodejs": "node.http.mjs",
        "@bzd_rules_nodejs//:nodejs_web": "window.fetch.mjs",
    }),
    output = "backend.mjs",
    visibility = ["//nodejs/core/http:__pkg__"],
)
