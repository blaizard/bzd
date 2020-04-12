load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_binary")

bzd_nodejs_binary(
    name = "backend",
    srcs = glob([
        "app.js",
    ]),
    deps = [
        "//platform/nodejs:web",
    ],
    main = "app.js",
    visibility = [
        "//apps/dashboard:__pkg__",
    ],
)
