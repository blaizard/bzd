load("//apps/dashboard/plugins:defs.bzl", "bzd_plugin")

bzd_plugin(
    name = "travisci",
    backend_deps = [
        "//nodejs/core/http:client",
    ],
    backend_entry_point = "source_backend.mjs",
    frontend_entry_point = "source_frontend.vue",
    frontend_srcs = [
        "travisci.svg",
    ],
    metadata = "plugin.mjs",
)
