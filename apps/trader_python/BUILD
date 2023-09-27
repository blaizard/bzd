load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "trader_python",
    srcs = [
        "trader_python.py",
    ],
    args = [
        "--frontend",
        "$(rootpath //apps/trader_python/frontend).bundle",
    ],
    data = [
        "//apps/trader_python/frontend",
    ],
    deps = [
        "//apps/trader_python/exchange/virtual",
        "//apps/trader_python/recording:recording_from_path",
        "@bzd_python//bzd/http:server",
    ],
)
