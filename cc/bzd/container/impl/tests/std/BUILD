load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "single_linked_list",
    srcs = [
        "single_linked_list.cc",
    ],
    linkopts = [
        "-pthread",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container/impl",
        "//cc/bzd/test:sync_point",
        "//cc/bzd/utility:ignore",
        "//tools/cc_test",
    ],
)
