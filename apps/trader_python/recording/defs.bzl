"""Override the configuration of the node manager application."""

load("@bzd_lib//config:defs.bzl", "make_bzd_config_apply")

bzd_generator_polygon_binary, _1 = make_bzd_config_apply(
    target = Label("//apps/trader_python/recording:generator_polygon"),
    configs = [
        Label("//apps/trader_python/recording:config_polygon"),
        Label("//apps/artifacts/api:config"),
    ],
    executable = True,
)

bzd_sp500_binary, _2 = make_bzd_config_apply(
    target = Label("//apps/trader_python/recording:sp500"),
    configs = [
        Label("//apps/artifacts/api:config"),
    ],
    executable = True,
)
