load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "non_owning_forward_list",
    timeout = "moderate",
    srcs = [
        "non_owning_forward_list.cc",
    ],
    tags = ["stress"],
    deps = [
        "//cc/bzd/container/threadsafe/tests:non_owning_forward_list_for_test",
        "//cc/bzd/test",
        "//cc/bzd/test:sync_point",
        "//cc/bzd/utility:ignore",
        "//cc/libs/pthread",
    ],
)

bzd_cc_test(
    name = "non_owning_queue_spin",
    timeout = "moderate",
    srcs = [
        "non_owning_queue_spin.cc",
    ],
    tags = ["stress"],
    deps = [
        ":utils",
        "//cc/bzd/container/threadsafe:non_owning_queue_spin",
        "//cc/bzd/test",
    ],
)

bzd_cc_test(
    name = "non_owning_ring_spin",
    timeout = "moderate",
    srcs = [
        "non_owning_ring_spin.cc",
    ],
    tags = ["stress"],
    deps = [
        ":utils",
        "//cc/bzd/container/threadsafe:non_owning_ring_spin",
        "//cc/bzd/test",
    ],
)

cc_library(
    name = "utils",
    hdrs = [
        "utils.hh",
    ],
    deps = [
        "//cc/bzd/container:vector",
        "//cc/bzd/platform:atomic",
        "//cc/libs/pthread",
    ],
)
