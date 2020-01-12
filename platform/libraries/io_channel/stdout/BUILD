"""
Such rule should generate a config.manifest with the following content:

interfaces:
    "bzd::io_channel::Stdout":
        deps: //platform/libraries/io_channel/stdout

It should also generate the C++ library

"""

load("@rules_cc//cc:defs.bzl", "cc_library")
load("@//tools/bzd:defs.bzl", "bzd_manifest")

bzd_manifest(
    name = "config_manifest",
    manifest = glob([
        "config.manifest",
    ]),
    deps = [],
)

cc_library(
    name = "stdout",
    hdrs = [
        "stdout.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":config_manifest",
    ],
)

"""
bzd_cc_library(
    name = "stdout",
	# Interfaces that are available from this library
	interfaces = [
		"bzd::io_channel::Stdout"
	],
	# Includes are automatically added to hdrs
	includes = [
		"libraries/stdout.h",
	],
	hdrs = [
		"stdout.h"
	],
    visibility = ["//visibility:public"],
    deps = [
        "//platform:bzd_core",
    ],
)
"""
