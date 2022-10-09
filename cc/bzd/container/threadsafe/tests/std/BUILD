load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "non_owning_forward_list",
    timeout = "long",
    testonly = True,
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
    name = "non_owning_queue",
    timeout = "long",
    testonly = True,
    srcs = [
        "non_owning_queue.cc",
    ],
    tags = ["stress"],
    deps = [
        "//cc/bzd/container/threadsafe:non_owning_queue_spin",
        "//cc/bzd/test",
        "//cc/bzd/test:exec_point",
        "//cc/libs/pthread",
    ],
)
