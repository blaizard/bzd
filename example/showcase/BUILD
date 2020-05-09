load("//tools/bazel.build/rules:nodejs_web.bzl", "bzd_nodejs_web")

bzd_nodejs_web(
    name = "showcase",
    srcs = glob([
        "**/*",
    ]),
    entries = {
        "app.mjs": "index",
    },
    packages = {
        "vue": "",
    },
    deps = [
        "//nodejs/styles/default",
        "//nodejs/vue/components/form",
        "//nodejs/vue/components/graph",
        "//nodejs/vue/components/layout",
        "//nodejs/vue/components/menu:entry",
        "//nodejs/vue/directives:loading",
        "//nodejs/vue/directives:tooltip",
        "//nodejs/vue/router",
    ],
)
