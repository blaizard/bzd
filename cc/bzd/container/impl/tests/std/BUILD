load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "non_owning_list",
    srcs = [
        "non_owning_list.cc",
    ],
    linkopts = [
        "-pthread",
    ],
    tags = ["stress"],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container/impl/tests:non_owning_list_lib",
        "//cc/bzd/test:sync_point",
        "//cc/bzd/utility:ignore",
        "//tools/cc_test",
    ],
)
