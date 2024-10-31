load("//apps/dashboard/plugins:defs.bzl", "bzd_plugin")

bzd_plugin(
    name = "sonos",
    backend_entry_point = "sonos_backend.mjs",
    backend_packages = [
        "@nodejs_deps//:chromecast-api",
        "@nodejs_deps//:sonos",
    ],
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
