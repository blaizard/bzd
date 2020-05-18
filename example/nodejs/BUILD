load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_binary")
load("//tools/bazel.build/rules:package.bzl", "bzd_package")

bzd_nodejs_binary(
    name = "nodejs",
    srcs = glob([
        "app.mjs",
    ]),
    main = "app.mjs",
    packages = {
        "mustache": "",
    },
    visibility = [
        "//apps/dashboard:__pkg__",
    ],
)

bzd_package(
    name = "package",
    tags = ["nodejs"],
    deps = {
        ":nodejs": "nodejs",
    },
)
