load("@//tools/bazel.build/bzd:defs.bzl", "bzd_manifest", "bzd_cc_binary")

filegroup(
    name = "bzd_files",
    srcs = glob([
        "bzd/**/*.h",
    ]),
    visibility = ["//visibility:public"],
)

bzd_manifest(
    name = "config_manifest",
    manifest = glob([
       "config.json",
    ]),
    deps = []
)

bzd_manifest(
    name = "config_manifest_temp",
    manifest = glob([
       "*.json",
    ]),
    deps = []
)

cc_library(
    name = "temp",
    hdrs = glob([
        "bzd/*.h",
    ]),
    deps = [
        ":config_manifest_temp"
    ],
)

cc_library(
    name = "bzd",
    hdrs = [
        ":bzd_files",
    ],
    deps = [
        ":config_manifest",
        ":temp"
    ],
    includes = ["."],
    visibility = ["//visibility:public"],
)

bzd_cc_binary(
    name = "hello",
    srcs = ["main.cpp"],
    deps = [
        ":bzd"
    ]
)
