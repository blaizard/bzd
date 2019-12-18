load("@//tools/bazel/bzd:defs.bzl", "bzd_cc_manifest", "bzd_cc_binary")

filegroup(
    name = "bzd_files",
    srcs = glob([
        "bzd/**/*.h",
    ]),
    visibility = ["//visibility:public"],
)

bzd_cc_manifest(
    name = "config_manifest",
    manifest = glob([
       "config.json",
    ]),
    deps = []
)

bzd_cc_manifest(
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

# bazel query 'kind(bzd_cc_manifest, deps(//include:bzd))'
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
    deps = [
        ":bzd"
    ]
)
