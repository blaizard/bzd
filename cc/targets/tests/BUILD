load("@bzd_bdl//:defs.bzl", "bdl_library")
load("@rules_cc//cc:defs.bzl", "cc_library")
load("//cc/bdl:cc.bzl", "bzd_cc_test")

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
    name = "lib",
    hdrs = [
        "timer.hh",
    ],
    deps = [
        ":composition",
        "//cc:bzd",
        "//cc/bzd/test:test_without_composition",
    ],
)

bzd_cc_test(
    name = "tests",
    srcs = [
        "timer.cc",
    ],
    deps = [
        ":lib",
    ],
)
