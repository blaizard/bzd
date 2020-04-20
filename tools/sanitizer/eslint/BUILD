load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_binary")

bzd_nodejs_binary(
    name = "eslint",
    srcs = glob([
        "*.js",
    ]),
    main = "app.js",
    args = [
        "--config", "tools/sanitizer/eslint/.eslintrc.json",
    ],
    data = [
        ".eslintrc.json",
    ],
    visibility = [
        "//tools/sanitizer:__pkg__",
    ],
    packages = {
        "eslint": "",
        "eslint-plugin-vue": "",
    }
)
