load("//apps/dashboard/plugins:defs.bzl", "bzd_plugin")

bzd_plugin(
    name = "jenkins",
    backend_deps = [
        "//nodejs/core/http:client",
    ],
    backend_entry_point = "source_backend.mjs",
    frontend_entry_point = "source_frontend.vue",
    frontend_srcs = [
        "jenkins.svg",
    ],
    metadata = "plugin.mjs",
)
