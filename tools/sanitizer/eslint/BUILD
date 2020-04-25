load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_binary")

bzd_nodejs_binary(
    name = "eslint",
    srcs = glob([
        "*.js",
    ]),
    args = [
        "--config",
        "tools/sanitizer/eslint/.eslintrc.json",
    ],
    data = [
        ".eslintrc.json",
    ],
    main = "app.js",
    packages = {
        "eslint": "",
        "eslint-plugin-vue": "",
    },
    visibility = [
        "//tools/sanitizer:__pkg__",
    ],
)
