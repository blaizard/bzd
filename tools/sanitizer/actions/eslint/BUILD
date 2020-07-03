load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_binary")

bzd_nodejs_binary(
    name = "eslint",
    srcs = glob([
        "*.cjs",
    ]),
    data = [
        ".eslintrc.json",
    ],
    main = "app.cjs",
    packages = {
        "eslint": "",
        "eslint-plugin-vue": "",
    },
    visibility = [
        "//tools/sanitizer:__pkg__",
    ],
)
