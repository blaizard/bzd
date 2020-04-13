load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_binary")

bzd_nodejs_binary(
    name = "backend",
    srcs = glob([
        "app.js",
    ]),
    main = "app.js",
    visibility = [
        "//apps/dashboard:__pkg__",
    ],
    deps = [
        "//platform/nodejs:web",
    ],
)
