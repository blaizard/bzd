load("@rules_cc//cc:defs.bzl", "cc_binary")
load("@//tools/bzd:defs.bzl", "bzd_pack")

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
