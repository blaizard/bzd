load("@pip//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_binary", "py_library")

py_binary(
    name = "generator_yfinance",
    srcs = [
        "generator_yfinance.py",
    ],
    tags = ["manual"],
    visibility = ["//apps/trader_python:__subpackages__"],
    deps = [
        requirement("yfinance"),
        ":recording_from_data",
        ":recording_to_path",
        "@bzd_python//bzd/utils:worker",
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
