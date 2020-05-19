load("@bazel_skylib//lib:selects.bzl", "selects")
load("@rules_cc//cc:defs.bzl", "cc_library")

filegroup(
    name = "documentation",
    srcs = glob([
        "bzd/**/*.h",
    ]),
    visibility = ["//visibility:public"],
)

filegroup(
    name = "headers",
    srcs = glob([
        "bzd/**/*.h",
    ]),
    visibility = [
        "//cc/libraries:__subpackages__",
    ],
)

cc_library(
    name = "header_system",
    hdrs = [
        "//cc:headers",
    ],
    includes = ["."],
    visibility = [
        "//cc/libraries:__subpackages__",
    ],
)

filegroup(
    name = "sources",
    srcs = glob([
        "bzd/**/*.cpp",
    ]),
    visibility = ["//visibility:private"],
)

cc_library(
    name = "bzd",
    hdrs = [
        "bzd.h",
        "//cc:headers",
    ],
    includes = ["."],
    visibility = ["//visibility:public"],
    deps = [
        ":bzd_core",
        ":libraries",
    ],
)

cc_library(
    name = "bzd_core",
    srcs = [
        "//cc:sources",
    ],
    hdrs = [
        "//cc:headers",
    ],
    includes = ["."],
    linkstatic = True,
    visibility = [
        "//cc/libraries:__subpackages__",
    ],
    deps = [
        "//cc/libraries/panic",
    ] + selects.with_or({
        (
            "//tools/bazel_build/config:x86_64",
        ): ["//cc/libraries/platforms/x86_64_cdecl"],
        "//conditions:default": [],
    }),
)

cc_library(
    name = "libraries",
    visibility = ["//visibility:private"],
    deps = [
        ":bzd_core",
        "//cc/libraries/log",
    ],
)
