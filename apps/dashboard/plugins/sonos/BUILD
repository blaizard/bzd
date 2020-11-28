load("//apps/dashboard/plugins:defs.bzl", "bzd_plugin")

bzd_plugin(
    name = "sonos",
    backend_packages = {
        "sonos": "",
    },
    backend_entry_point = "sonos_backend.mjs",
    frontend_entry_point = "sonos_frontend.vue",
    frontend_srcs = [
        "sonos.svg",
    ],
    metadata = "sonos.mjs",
)

bzd_plugin(
    name = "media_player",
    frontend_entry_point = "media_player.vue",
    metadata = "media_player.mjs",
)
