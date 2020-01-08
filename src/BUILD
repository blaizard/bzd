load("@bazel_skylib//lib:selects.bzl", "selects")
load("@rules_cc//cc:defs.bzl", "cc_library")
load("@//tools/bzd:defs.bzl", "bzd_cc_binary", "bzd_manifest")

filegroup(
    name = "headers",
    srcs = glob([
        "**/*.h",
    ]),
    visibility = ["//visibility:public"],
)

filegroup(
    name = "sources",
    srcs = glob([
        "**/*.cpp",
    ]),
    visibility = ["//visibility:public"],
)

cc_library(
    name = "bzd",
    hdrs = [
        "bzd.h",
        "//src/bzd:headers",
    ],
    includes = ["."],
    visibility = ["//visibility:public"],
    deps = [
        ":bzd_core",
        ":bzd_lib",
    ],
)

cc_library(
    name = "bzd_core",
    srcs = [
        "//src/bzd:sources",
    ],
    hdrs = [
        "bzd.h",
        "//src/bzd:headers",
    ],
    includes = ["."],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "bzd_lib",
    visibility = ["//visibility:public"],
    deps = [
        ":bzd_core",
        "//src/bzd_lib/log",
        "//src/bzd_lib/panic",
    ] + selects.with_or({
        (
            "//tools/bazel.build/config:x86_64",
            "//tools/bazel.build/config/auto:x86_64",
        ): ["//src/bzd_lib/platforms/x86_64_cdecl"],
        "//conditions:default": [],
    }),
)
