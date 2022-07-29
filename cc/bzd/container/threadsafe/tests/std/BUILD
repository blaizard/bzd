load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

cc_library(
    name = "thread",
    testonly = True,
    linkopts = [
        "-pthread",
    ],
    target_compatible_with = select({
        "@esp32_xtensa_lx6_gcc//:compiler": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
)

bzd_cc_test(
    name = "non_owning_forward_list",
    testonly = True,
    srcs = [
        "non_owning_forward_list.cc",
    ],
    tags = ["stress"],
    deps = [
        ":thread",
        "//cc/bzd/container/threadsafe/tests:non_owning_forward_list_for_test",
        "//cc/bzd/test",
        "//cc/bzd/test:sync_point",
        "//cc/bzd/utility:ignore",
    ],
)

bzd_cc_test(
    name = "non_owning_queue",
    testonly = True,
    srcs = [
        "non_owning_queue.cc",
    ],
    tags = ["stress"],
    deps = [
        ":thread",
        "//cc/bzd/container/threadsafe:non_owning_queue",
        "//cc/bzd/test",
        "//cc/bzd/test:exec_point",
    ],
)
