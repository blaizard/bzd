load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "trader_python",
    srcs = [
        "trader_python.py",
    ],
    deps = [
        "//apps/trader_python/exchange/virtual",
        "//apps/trader_python/recording",
    ],
)
