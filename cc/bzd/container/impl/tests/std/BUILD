load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

cc_library(
    name = "thread",
    testonly = True,
    linkopts = [
        "-pthread",
    ],
)

bzd_cc_test(
    name = "non_owning_list",
    testonly = True,
    srcs = [
        "non_owning_list.cc",
    ],
    tags = ["stress"],
    deps = [
        ":thread",
        "//cc/bzd/container/impl/tests:non_owning_list_for_test",
        "//cc/bzd/test",
        "//cc/bzd/test:sync_point",
        "//cc/bzd/utility:ignore",
    ],
)
