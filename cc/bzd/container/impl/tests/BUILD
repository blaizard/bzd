load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "cc_test")

cc_library(
    name = "non_owning_list_lib",
    hdrs = [
        "non_owning_list.h",
    ],
    visibility = ["//cc/bzd/container/impl/tests:__subpackages__"],
    deps = [
        "//cc/bzd/container/impl:non_owning_list",
        "//cc/bzd/platform",
        "//cc/bzd/utility/format",
    ],
)

cc_test(
    name = "non_owning_list",
    srcs = [
        "non_owning_list.cc",
        "non_owning_list.h",
    ],
    tags = ["stress"],
    deps = [
        "//tools/cc_test",
        ":non_owning_list_lib",
    ] + select({
        "//tools/bazel_build/config:x86_64": [
            "//cc/bzd/container/impl/tests/std:non_owning_list",
        ],
        "//conditions:default": [],
    }),
)
