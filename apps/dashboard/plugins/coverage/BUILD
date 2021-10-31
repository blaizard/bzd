load("//apps/dashboard/plugins:defs.bzl", "bzd_plugin")

bzd_plugin(
    name = "coverage_report",
    backend_entry_point = "source_backend.mjs",
    metadata = "report.mjs",
)

bzd_plugin(
    name = "coverage",
    frontend_entry_point = "visualization.vue",
    metadata = "coverage.mjs",
    frontend_deps = [
        "//nodejs/vue/directives:tooltip",
    ],
    frontend_srcs = [
        "branches.svg",
        "functions.svg",
        "lines.svg",
    ],
)
