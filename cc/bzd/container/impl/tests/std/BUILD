load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "doubly_linked_list",
    srcs = [
        "doubly_linked_list.cc",
    ],
    linkopts = [
        "-pthread",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container/impl",
        "//cc/bzd/test:sync_point",
        "//tools/cc_test",
    ],
)
