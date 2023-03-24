load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "tests",
    srcs = glob(["*.cc"]),
    tags = ["manual"],
    deps = [
        "//cc/bzd/test:multithread",
        "//cc/bzd/test:sync_barrier",
        "//cc/libs/timer/tests/std:timer",
    ],
)

cc_library(
    name = "timer",
    testonly = True,
    hdrs = ["timer.hh"],
    visibility = [
        "//cc/libs/timer/tests:__pkg__",
    ],
    deps = [
        "//cc/bzd/platform:atomic",
        "//cc/bzd/utility:numeric_limits",
        "//cc/libs/pthread",
        "//cc/libs/timer:timer_isr",
    ],
)
