load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

cc_library(
    name = "non_owning_forward_list_for_test",
    testonly = True,
    hdrs = [
        "non_owning_forward_list_for_test.hh",
    ],
    visibility = ["//cc/bzd/container/threadsafe/tests:__subpackages__"],
    deps = [
        "//cc/bzd/container/threadsafe:non_owning_forward_list",
        "//cc/bzd/core/assert",
        "//cc/bzd/utility/format",
    ],
)

bzd_cc_test(
    name = "non_owning_forward_list",
    srcs = [
        "non_owning_forward_list.cc",
    ],
    tags = ["stress"],
    deps = [
        ":non_owning_forward_list_for_test",
        "//cc/bzd/test",
    ],
)

bzd_cc_test(
    name = "non_owning_queue",
    srcs = [
        "non_owning_queue.cc",
    ],
    deps = [
        "//cc/bzd/container/threadsafe:non_owning_queue_spin",
        "//cc/bzd/test",
        "//cc/bzd/test:exec_point",
    ],
)
