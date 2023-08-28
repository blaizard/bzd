load("@bzd_toolchain_nodejs//nodejs_web:defs.bzl", "bzd_nodejs_web_binary")

bzd_nodejs_web_binary(
    name = "showcase",
    srcs = glob([
        "**/*",
    ]),
    main = "app.mjs",
    packages = {
        "vue": "",
    },
    deps = [
        "//nodejs/styles/default",
        "//nodejs/vue:notification",
        "//nodejs/vue/components/form",
        "//nodejs/vue/components/graph",
        "//nodejs/vue/components/layout",
        "//nodejs/vue/components/menu:entry",
        "//nodejs/vue/directives:loading",
        "//nodejs/vue/directives:tooltip",
        "//nodejs/vue/router",
    ],
)
