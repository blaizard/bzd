load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_binary")
load("//tools/bazel_build/rules:bdl.bzl", "bzd_manifest")

bzd_manifest(
    name = "interface",
    srcs = [
        "manifest.bdl",
    ],
)

bzd_cc_binary(
    name = "manifest",
    srcs = [
        "main.cc",
    ],
    deps = [
        ":interface",
    ],
)
