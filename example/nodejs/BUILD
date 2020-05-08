load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_binary")

bzd_nodejs_binary(
    name = "nodejs",
    srcs = glob([
        "app.mjs",
    ]),
    main = "app.mjs",
    packages = {
        "mustache": "latest",
    },
    visibility = [
        "//apps/dashboard:__pkg__",
    ],
)
