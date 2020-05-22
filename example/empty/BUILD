load("@rules_cc//cc:defs.bzl", "cc_binary")
load("@//tools/bazel_build/rules:cc.bzl", "bzd_pack")

cc_binary(
    name = "empty",
    srcs = [
        "main.cpp",
    ],
)

bzd_pack(
    name = "deploy",
    binary = ":empty",
)
