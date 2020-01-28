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
        "//platform/libraries:__subpackages__",
    ],
)

filegroup(
    name = "header_system",
    srcs = glob([
        "bzd/core/system.h",
    ]),
    visibility = [
        "//platform/libraries:__subpackages__",
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
    name = "empty_sdk",
)

alias(
    name = "sdk",
    actual = select({
        "@esp32_xtensa_lx6_gcc//:target": "@esp32_xtensa_lx6_gcc//:sdk",
        "//conditions:default": ":empty_sdk",
    }),
    visibility = ["//visibility:public"],
)

cc_library(
    name = "bzd",
    hdrs = [
        "bzd.h",
        "//platform:headers",
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
        "//platform:sources",
    ],
    hdrs = [
        "//platform:headers",
    ],
    includes = ["."],
    visibility = [
        "//platform/libraries:__subpackages__",
    ],
)

cc_library(
    name = "libraries",
    visibility = ["//visibility:private"],
    deps = [
        ":bzd_core",
        "//platform/libraries/log",
        "//platform/libraries/panic",
    ] + selects.with_or({
        (
            "//tools/bazel.build/config:x86_64",
            "//tools/bazel.build/config/auto:x86_64",
        ): ["//platform/libraries/platforms/x86_64_cdecl"],
        "//conditions:default": [],
    }),
)
