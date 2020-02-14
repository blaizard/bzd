load("@//tools/bzd:defs.bzl", "bzd_cc_binary", "bzd_manifest", "bzd_pack")

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
        "//platform:sdk",
        "//platform/libraries/io/digital",
        "//platform/libraries/io/stdout",
    ],
)

bzd_pack(
    name = "deploy",
    binary = ":bzd",
)
