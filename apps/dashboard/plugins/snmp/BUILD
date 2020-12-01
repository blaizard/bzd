load("//apps/dashboard/plugins:defs.bzl", "bzd_plugin")

bzd_plugin(
    name = "snmp",
    backend_entry_point = "snmp_backend.mjs",
    backend_packages = {
        "net-snmp": "",
    },
    metadata = "plugin.mjs",
)
