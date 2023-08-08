load("@bzd_toolchain_nodejs//nodejs:defs.bzl", "bzd_nodejs_binary")

bzd_nodejs_binary(
    name = "prettier",
    srcs = [
        "app.mjs",
    ],
    main = "app.mjs",
    packages = {
        "prettier": "",
    },
    visibility = [
        "//tools/sanitizer:__pkg__",
    ],
    deps = [
        "//nodejs/core:filesystem",
        "//tools/sanitizer/utils/nodejs:workspace",
    ],
)
