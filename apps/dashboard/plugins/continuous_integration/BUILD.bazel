load("//apps/dashboard/plugins:defs.bzl", "bzd_plugin")

bzd_plugin(
    name = "continuous_integration",
    frontend_deps = [
        "//nodejs/vue/directives:tooltip",
        "//nodejs/vue/components/graph",
    ],
    frontend_entry_point = "visualization.vue",
    frontend_srcs = [
        "continuous_integration.svg",
    ],
    metadata = "plugin.mjs",
)
