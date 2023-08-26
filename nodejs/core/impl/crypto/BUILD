load("@bzd_utils//:symlink.bzl", "bzd_symlink")

bzd_symlink(
    name = "backend",
    input = select({
        "@bzd_toolchain_nodejs//:nodejs": "buffer.base64.mjs",
        "@bzd_toolchain_nodejs//:nodejs_web": "window.btoa.mjs",
    }),
    output = "backend.mjs",
    visibility = ["//nodejs/core:__pkg__"],
)
