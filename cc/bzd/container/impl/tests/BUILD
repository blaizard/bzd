load("//tools/bazel_build/rules:cc.bzl", "cc_test")

cc_test(
    name = "doubly_linked_list",
    srcs = [
        "doubly_linked_list.cc",
    ],
    deps = [
        "//cc/bzd/container/impl",
        "//tools/cc_test",
    ] + select({
        "//tools/bazel_build/config:x86_64": [
            "//cc/bzd/container/impl/tests/std:doubly_linked_list",
        ],
        "//conditions:default": [],
    }),
)
