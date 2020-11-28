load("//apps/dashboard/plugins:defs.bzl", "bzd_plugin")

bzd_plugin(
    name = "continuous_integration",
    metadata = "plugin.mjs",
    frontend_entry_point = "visualization.vue",
    frontend_srcs = [
        "continuous_integration.svg",
    ],
    frontend_deps = [
        "//nodejs/vue/components/graph",
    ],
)
