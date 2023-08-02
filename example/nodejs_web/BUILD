load("//tools/bazel_build/rules:nodejs_web.bzl", "bzd_nodejs_web")

bzd_nodejs_web(
    name = "nodejs_web",
    srcs = [
        "app.mjs",
        "app.vue",
    ],
    entries = {
        "app.mjs": "index",
    },
    packages = {
        "@vue/composition-api": "latest",
    },
    deps = [
        "//nodejs:vue",
    ],
)
