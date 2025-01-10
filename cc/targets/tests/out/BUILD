load("@bzd_bdl//:defs.bzl", "bdl_library")
load("@bzd_lib//:executable_test.bzl", "bzd_executable_exec_test")
load("@rules_cc//cc:defs.bzl", "cc_library")
load("//cc/bdl:cc.bzl", "bzd_cc_binary")

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

bzd_cc_binary(
    name = "out_binary",
    testonly = True,
    srcs = [
        "out.cc",
    ],
    deps = [
        ":runner",
    ],
)

bzd_executable_exec_test(
    name = "out",
    executable = ":out_binary",
    expected_output = "TEST MESSAGE 95485743",
    tags = ["cc"],
)
