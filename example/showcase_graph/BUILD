load("//tools/bazel.build/rules:nodejs_web.bzl", "bzd_nodejs_web")

bzd_nodejs_web(
    name = "showcase_graph",
    srcs = glob([
        "app.vue",
        "app.js",
    ]),
    entries = {
        "app.js": "index",
    },
    packages = {
        "vue": "",
    },
    deps = [
        "//nodejs/styles/default",
        "//nodejs/vue/components/graph",
    ],
)
