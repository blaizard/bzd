load("@bzd_toolchain_nodejs//nodejs_web:defs.bzl", "bzd_nodejs_web")

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
