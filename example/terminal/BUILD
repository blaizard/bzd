load("@bzd_bdl//:defs.bzl", "bdl_library")
load("@rules_cc//cc:defs.bzl", "cc_library")
load("//cc/bdl:cc.bzl", "bzd_cc_binary")

bdl_library(
    name = "composition",
    srcs = [
        "composition.bdl",
    ],
    deps = [
        "//interfaces:stream",
    ],
)

cc_library(
    name = "reader",
    hdrs = [
        "reader.hh",
    ],
    visibility = [
        "//example/terminal/tests:__pkg__",
    ],
    deps = [
        "//cc:bzd",
    ],
)

cc_library(
    name = "lib",
    srcs = [
        "terminal.cc",
    ],
    hdrs = [
        "terminal.hh",
    ],
    deps = [
        ":composition",
        ":reader",
        "//cc:bzd",
    ],
)

bzd_cc_binary(
    name = "terminal",
    deps = [
        ":lib",
    ],
)
