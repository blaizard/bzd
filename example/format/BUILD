load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_binary")
load("//tools/bazel_build/rules:package.bzl", "bzd_package")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

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
