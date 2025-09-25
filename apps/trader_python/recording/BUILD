load("@bzd_lib//config:defs.bzl", "bzd_config_default")
load("@bzd_python//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_binary", "py_library")

bzd_config_default(
    name = "config_polygon",
    srcs = [
        "config.polygon.json",
    ],
    visibility = [
        "//deployment:__subpackages__",
    ],
)

py_binary(
    name = "generator_yfinance",
    srcs = [
        "generator_yfinance.py",
    ],
    tags = ["manual"],
    visibility = ["//apps/trader_python:__subpackages__"],
    deps = [
        ":recording_from_data",
        ":recording_to_path",
        "@bzd_python//bzd/utils:worker",
    ] + requirement("yfinance"),
)

py_binary(
    name = "generator_polygon",
    srcs = [
        "config_polygon.py",
        "generator_polygon.py",
    ],
    tags = ["manual"],
    visibility = [
        "//apps/trader_python:__subpackages__",
        "//deployment:__subpackages__",
    ],
    deps = [
        "//apps/artifacts/api/python/node",
        "@bzd_python//bzd/http:client",
        "@bzd_python//bzd/logging",
    ],
)

py_library(
    name = "recording_from_data",
    srcs = [
        "recording_from_data.py",
    ],
    visibility = ["//apps/trader_python:__subpackages__"],
    deps = [
        ":recording",
    ],
)

py_library(
    name = "recording_from_path",
    srcs = [
        "recording_from_path.py",
    ],
    visibility = ["//apps/trader_python:__subpackages__"],
    deps = [":recording"],
)

py_library(
    name = "recording_to_path",
    srcs = [
        "recording_to_path.py",
    ],
    visibility = ["//apps/trader_python:__subpackages__"],
    deps = [
        ":recording",
        ":recording_from_data",
        ":recording_from_path",
    ],
)

py_library(
    name = "recording",
    srcs = [
        "recording.py",
    ],
    visibility = ["//apps/trader_python:__subpackages__"],
)

py_binary(
    name = "sp500",
    srcs = [
        "sp500.py",
    ],
    visibility = [
        "//deployment:__subpackages__",
    ],
    deps = [
        "//apps/artifacts/api/python/node",
        "@bzd_python//bzd/http:client",
    ] + requirement("lxml"),
)
