load("//tools/bazel_build/rules:cc.bzl", "cc_test")

cc_test(
    name = "non_owning_list",
    srcs = [
        "non_owning_list.cc",
    ],
    deps = [
        "//cc/bzd/container/impl",
        "//tools/cc_test",
    ] + select({
        "//tools/bazel_build/config:x86_64": [
            "//cc/bzd/container/impl/tests/std:non_owning_list",
        ],
        "//conditions:default": [],
    }),
)
