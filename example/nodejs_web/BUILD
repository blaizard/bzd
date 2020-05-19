load("//tools/bazel_build/rules:nodejs_web.bzl", "bzd_nodejs_web")

bzd_nodejs_web(
    name = "nodejs_web",
    srcs = glob([
        "app.vue",
        "app.mjs",
    ]),
    entries = {
        "app.mjs": "index",
    },
    packages = {
        "vue": "latest",
        "@vue/composition-api": "latest",
    },
)
