load("@bzd_package//:defs.bzl", "bzd_package")
load("@bzd_toolchain_nodejs//nodejs_web:defs.bzl", "bzd_nodejs_web")

bzd_nodejs_web(
    name = "frontend",
    srcs = glob([
        "**/*.vue",
        "**/*.svg",
        "**/*.mjs",
    ]),
    entries = {
        "app.mjs": "index",
    },
    packages = {
        "vue-async-computed": "",
    },
    visibility = [
        "//apps/artifacts:__pkg__",
    ],
    deps = [
        "//apps/artifacts:api",
        "//apps/artifacts/plugins:frontend",
        "//nodejs:icons",
        "//nodejs:vue",
        "//nodejs/core/authentication/token:client",
        "//nodejs/db/storage:permissions",
        "//nodejs/styles/default",
        "//nodejs/vue:api",
        "//nodejs/vue:authentication",
        "//nodejs/vue:cache",
        "//nodejs/vue:notification",
        "//nodejs/vue/components/form",
        "//nodejs/vue/components/layout",
        "//nodejs/vue/components/layout:component",
        "//nodejs/vue/components/menu:entry",
        "//nodejs/vue/directives:loading",
        "//nodejs/vue/directives:tooltip",
        "//nodejs/vue/router",
    ],
)

bzd_package(
    name = "local",
    args = ["/tmp/bzd"],
    include_metadata = True,
    tags = ["nodejs"],
    deps = {
        ":frontend": "frontend",
    },
)
