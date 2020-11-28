load("//apps/dashboard/plugins:defs.bzl", "bzd_plugin")

bzd_plugin(
    name = "github",
    backend_deps = [
        "//nodejs/core/http:client",
    ],
    backend_entry_point = "source_backend.mjs",
    frontend_entry_point = "source_frontend.vue",
    frontend_srcs = [
        "github.svg",
    ],
    metadata = "plugin.mjs",
)
