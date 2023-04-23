load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "bitset",
    srcs = [
        "bitset.cc",
    ],
    deps = [
        "//cc/bzd/container/threadsafe:bitset",
        "//cc/bzd/test",
    ],
)

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
        "//cc/bzd/platform:stream",
        "//cc/bzd/utility/pattern/formatter:to_string",
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
    name = "non_owning_queue_spin",
    srcs = [
        "non_owning_queue_spin.cc",
    ],
    deps = [
        "//cc/bzd/container/threadsafe:non_owning_queue_spin",
        "//cc/bzd/test",
    ],
)

bzd_cc_test(
    name = "non_owning_ring_spin",
    srcs = [
        "non_owning_ring_spin.cc",
    ],
    deps = [
        "//cc/bzd/container/threadsafe:non_owning_ring_spin",
        "//cc/bzd/test",
    ],
)

bzd_cc_test(
    name = "ring_buffer",
    srcs = [
        "ring_buffer.cc",
    ],
    deps = [
        "//cc/bzd/container/threadsafe:ring_buffer",
        "//cc/bzd/test",
    ],
)
