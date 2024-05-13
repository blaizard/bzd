load("@bzd_bdl//:defs.bzl", "bdl_library")
load("@bzd_package//:defs.bzl", "bzd_package")
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
    name = "lib",
    srcs = [
        "format.cc",
    ],
    hdrs = [
        "format.hh",
    ],
    deps = [
        ":composition",
        "//cc:bzd",
    ],
)

bzd_cc_binary(
    name = "format",
    deps = [
        ":lib",
    ],
)

bzd_package(
    name = "package",
    include_metadata = True,
    tags = ["manual"],
    deps = {
        ":format": "format",
    },
)
