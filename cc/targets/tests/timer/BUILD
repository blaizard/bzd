load("@bzd_bdl//:defs.bzl", "bdl_library")
load("@bzd_lib//:executable_test.bzl", "bzd_executable_test")
load("@rules_cc//cc:defs.bzl", "cc_library")
load("//cc/bdl:cc.bzl", "bzd_cc_binary", "bzd_cc_test")

package(default_testonly = True)

bdl_library(
    name = "composition",
    srcs = [
        "composition.bdl",
    ],
    deps = [
        "//cc/bzd/test:runner",
        "//interfaces:timer",
    ],
)

cc_library(
    name = "runner",
    hdrs = [
        "runner.hh",
    ],
    deps = [
        ":composition",
        "//cc:bzd",
        "//cc/bzd/test:test_without_composition",
    ],
)

bzd_cc_test(
    name = "timer",
    srcs = [
        "timer.cc",
    ],
    deps = [
        ":runner",
    ],
)

bzd_cc_binary(
    name = "duration_binary",
    testonly = True,
    srcs = [
        "duration.cc",
    ],
    deps = [
        ":runner",
    ],
)

bzd_executable_test(
    name = "duration",
    executable = ":duration_binary",
    min_duration = 0.8,
)
