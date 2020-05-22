"""
Such rule should generate a config.manifest with the following content
and will generate a generate the C++ library

interfaces:
    "bzd::io_channel::Stdout":
        includes: libraries/io/stdout/stdout.h
"""

load("@//tools/bazel_build/rules:cc.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "stdout",
    hdrs = [
        "stdout.h",
    ],
    includes = [
        "libraries/io/stdout/stdout.h",
    ],
    interfaces = {
        "bzd::io::Stdout": "*",
    },
    visibility = ["//visibility:public"],
    deps = [
        "//cc:bzd_core",
    ],
)
