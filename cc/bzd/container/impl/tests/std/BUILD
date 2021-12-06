load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "non_owning_list",
    srcs = [
        "non_owning_list.cc",
    ],
    linkopts = [
        "-pthread",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container/impl/tests:non_owning_list_for_test",
        "//cc/bzd/test",
        "//cc/bzd/test:sync_point",
        "//cc/bzd/utility:ignore",
    ],
)
