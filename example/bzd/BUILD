load("//tools/bazel_build/rules:manifest.bzl", "bzd_manifest")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_binary")
load("//tools/bazel_build/rules:package.bzl", "bzd_package")

bzd_manifest(
    name = "config_manifest",
    artifacts = {
        "message.txt": "test",
    },
    manifest = glob([
        "config.yml",
    ]) + select({
        "@esp32_xtensa_lx6_gcc//:target": ["config_esp32_xtensa_lx6.yml"],
        "//conditions:default": ["config_others.yml"],
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

bzd_package(
    name = "package",
    include_metadata = True,
    deps = {
        ":bzd": "bzd",
    },
)
