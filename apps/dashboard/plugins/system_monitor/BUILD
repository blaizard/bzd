load("//apps/dashboard/plugins:defs.bzl", "bzd_plugin")

bzd_plugin(
    name = "system_monitor",
    frontend_deps = [
        "//nodejs/vue/directives:tooltip",
        "//nodejs/utils:to_string",
        "//nodejs/core:exception",
    ],
    frontend_entry_point = "system_monitor.vue",
    frontend_srcs = [
        "gauge.vue",
        "gauge_rate.vue",
        "text.vue",
    ],
    metadata = "plugin.mjs",
)
