load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

cc_library(
    name = "non_owning_list_for_test",
    hdrs = [
        "non_owning_list_for_test.hh",
    ],
    visibility = ["//cc/bzd/container/impl/tests:__subpackages__"],
    deps = [
        "//cc/bzd/container/impl:non_owning_list",
        "//cc/bzd/core/assert",
        "//cc/bzd/platform",
        "//cc/bzd/utility/format",
    ],
)

bzd_cc_test(
    name = "non_owning_list",
    srcs = [
        "non_owning_list.cc",
    ],
    tags = ["stress"],
    deps = [
        "//tools/cc_test",
        ":non_owning_list_for_test",
    ] + select({
        "//tools/bazel_build/config:x86_64": [
            "//cc/bzd/container/impl/tests/std:non_owning_list",
        ],
        "//conditions:default": [],
    }),
)
