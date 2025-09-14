"""Override the configuration of the node manager application."""

load("@bzd_lib//config:defs.bzl", "make_bzd_config_apply")

bzd_generator_polygon_binary, _ = make_bzd_config_apply(
    target = Label("//apps/trader_python/recording:generator_polygon"),
    configs = [
        Label("//apps/trader_python/recording:config_polygon"),
        Label("//apps/artifacts/api:config"),
    ],
    executable = True,
)
