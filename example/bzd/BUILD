load("@//tools/bazel.build/bzd:defs.bzl", "bzd_cc_binary", "bzd_manifest")

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
    deps = [
        ":config_manifest",
        "//src:bzd",
    ],
)
