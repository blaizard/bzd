"""
Such rule should generate a config.manifest with the following content:

interfaces:
    "bzd::io_channel::Stdout":
        deps: //platform/libraries/io_channel/stdout

It should also generate the C++ library

"""

load("@//tools/bzd:defs.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "stdout",
    hdrs = [
        "stdout.h",
    ],
    includes = [
        "libraries/io/stdout/stdout.h",
    ],
    interfaces = [
        "bzd::io::Stdout",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//platform:bzd_core",
    ],
)
