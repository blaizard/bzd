load("//apps/dashboard/plugins:defs.bzl", "bzd_plugin")

bzd_plugin(
    name = "github",
    metadata = "plugin.mjs",
    frontend_entry_point = "source_frontend.vue",
    frontend_srcs = [
        "github.svg",
    ],
    backend_entry_point = "source_backend.mjs",
    backend_deps = [
        "//nodejs/core/http:client",
    ],
)
