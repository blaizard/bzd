load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_binary")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
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
        "//cc/bzd/type_traits:function",
        "//cc/bzd/type_traits:is_same",
    ],
)

bzd_cc_binary(
    name = "test",
    srcs = [
        "test.cc",
    ],
    tags = ["manual"],
)
