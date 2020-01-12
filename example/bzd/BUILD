load("@//tools/bzd:defs.bzl", "bzd_cc_binary", "bzd_manifest")

bzd_manifest(
    name = "config_manifest",
    manifest = glob([
        "config.manifest",
    ]),
    deps = [],
)

bzd_cc_binary(
    name = "bzd",
    srcs = [
        "main.cpp",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":config_manifest",
        "//platform:bzd",
        "//platform/libraries/io_channel/stdout"
    ],
)
