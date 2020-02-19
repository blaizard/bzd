load("@//tools/bzd:defs.bzl", "bzd_cc_binary", "bzd_manifest", "bzd_pack")

bzd_manifest(
    name = "config_manifest",
    manifest = glob([
        "config.manifest",
    ]) + select({
        "@esp32_xtensa_lx6_gcc//:target": ["config_esp32_xtensa_lx6.manifest"],
        "//conditions:default": ["config_others.manifest"],
    }),
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
        "//platform/libraries/io/digital",
        "//platform/libraries/io/stdout",
    ],
)

bzd_pack(
    name = "deploy",
    binary = ":bzd",
)
