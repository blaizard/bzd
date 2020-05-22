load("@//tools/bazel_build/rules:manifest.bzl", "bzd_manifest")
load("@//tools/bazel_build/rules:cc.bzl", "bzd_cc_binary")

bzd_manifest(
    name = "config_manifest",
    artifacts = {
        "message.txt": "test",
    },
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
        "//cc:bzd",
        "//cc/libraries/io/digital",
        "//cc/libraries/io/i2c",
        "//cc/libraries/io/stdout",
        "//cc/libraries/io/stub",
    ],
)
