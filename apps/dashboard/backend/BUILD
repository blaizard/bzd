load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_binary")

bzd_nodejs_binary(
    name = "backend",
    srcs = glob([
        "*.js",
    ]),
    main = "app.js",
    args = [
        "--static", "/tmp/frontend",
    ],
    visibility = [
        "//apps/dashboard:__pkg__",
    ],
    deps = [
        "//nodejs/require:key_value_store",
        "//nodejs/require:web",
    ],
)
