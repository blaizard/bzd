load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_binary")

bzd_nodejs_binary(
    name = "nodejs",
    srcs = glob([
        "app.js",
    ]),
    main = "app.js",
    packages = {
        "mustache": "latest",
    },
)
