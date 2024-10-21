load("//apps/dashboard/plugins:defs.bzl", "bzd_plugin")

bzd_plugin(
    name = "nodes",
    backend_deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/db:utils",
        "//apps/artifacts/api/nodejs/node",
    ],
    backend_entry_point = "backend.mjs",
    metadata = "plugin.mjs",
)
