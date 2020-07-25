load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_binary")

bzd_nodejs_binary(
    name = "prettier",
    srcs = glob([
        "*.mjs",
    ]),
    data = [
        "prettier.json",
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
