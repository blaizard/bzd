load("@bzd_utils//:symlink.bzl", "bzd_symlink")

bzd_symlink(
    name = "backend",
    input = select({
        "@bzd_toolchain_nodejs//:nodejs": "node.http.mjs",
        "@bzd_toolchain_nodejs//:nodejs_web": "window.fetch.mjs",
    }),
    output = "backend.mjs",
    visibility = ["//nodejs/core/http:__pkg__"],
)
