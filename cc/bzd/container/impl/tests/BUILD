load("//tools/bazel_build/rules:cc.bzl", "cc_test")

cc_test(
    name = "circular_list",
    srcs = [
        "circular_list.cc",
    ],
    deps = [
        "//cc/bzd/container/impl",
        "//tools/cc_test",
    ] + select({
        "//tools/bazel_build/config:x86_64": [
            "//cc/bzd/container/impl/tests/std:circular_list",
        ],
        "//conditions:default": [],
    }),
)
