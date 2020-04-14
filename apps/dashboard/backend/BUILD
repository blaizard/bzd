load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_binary")

bzd_nodejs_binary(
    name = "backend",
    srcs = glob([
        "*.js",
    ]),
    main = "app.js",
    visibility = [
        "//apps/dashboard:__pkg__",
    ],
    deps = [
        "//platform/nodejs/require:web",
        "//platform/nodejs/require:persistence",
    ],
)
