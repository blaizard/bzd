load("@//tools/bzd:defs.bzl", "bzd_cc_binary", "bzd_manifest", "bzd_pack")

cc_binary(
    name = "wifi",
    srcs = [
        "main.cpp",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//platform:bzd",
    ],
)

bzd_pack(
    name = "deploy",
    binary = ":wifi",
)
