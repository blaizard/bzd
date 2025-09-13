load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "timeseries_collection",
    srcs = [
        "timeseries_collection.mjs",
    ],
    visibility = [
        "//apps/artifacts/plugins:__subpackages__",
    ],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
    ],
)

bzd_nodejs_library(
    name = "view",
    srcs = [
        "keys.vue",
        "value.vue",
        "view.vue",
        "view_dashboard.vue",
        "view_data.vue",
        "view_graph.vue",
    ],
    packages = [
        "@nodejs_deps//:chart-js",
        "@nodejs_deps//:chartjs-adapter-date-fns",
        "@nodejs_deps//:chartjs-plugin-zoom",
        "@nodejs_deps//:date-fns",
    ],
    visibility = [
        "//apps/artifacts/plugins:__subpackages__",
    ],
    deps = [
        ":timeseries_collection",
        "//apps/artifacts/common:utils",
        "//apps/artifacts/plugins:default",
        "//nodejs/core:localstorage",
        "//nodejs/core:lock",
        "//nodejs/utils:array",
        "//nodejs/utils:to_string",
        "//nodejs/vue/components/form",
        "//nodejs/vue/directives:tooltip",
    ],
)
