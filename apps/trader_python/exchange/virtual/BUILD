load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "virtual",
    srcs = [
        "exchange.py",
        "pair.py",
    ],
    visibility = ["//apps/trader_python:__pkg__"],
    deps = [
        "//apps/trader_python/exchange",
        "//apps/trader_python/recording",
    ],
)
