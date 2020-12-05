load("//apps/dashboard/plugins:defs.bzl", "bzd_plugin")

bzd_plugin(
    name = "snmp",
    backend_deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
    ],
    backend_entry_point = "snmp_backend.mjs",
    backend_packages = {
        "net-snmp": "",
    },
    metadata = "plugin.mjs",
)
