load("@bzd_utils//:symlink.bzl", "bzd_symlink")

bzd_symlink(
    name = "backend",
    input = select({
        "@bzd_toolchain_nodejs//:nodejs": "process.mjs",
        "@bzd_toolchain_nodejs//:nodejs_web": "window.mjs",
    }),
    output = "backend.mjs",
    visibility = ["//nodejs/core:__pkg__"],
)
