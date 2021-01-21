load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_binary")
load("//tools/bazel_build/rules:bdl.bzl", "bzd_manifest")

bzd_manifest(
    name = "manifest_file",
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
        ":manifest_file",
    ],
)
