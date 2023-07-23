load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "interface",
    srcs = [
        "interface.bdl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/components/posix/proactor",
    ],
)

cc_library(
    name = "epoll",
    srcs = [
        "proactor.cc",
    ],
    hdrs = [
        "proactor.hh",
    ],
    target_compatible_with = select({
        "@esp32//:toolchain": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
    visibility = ["//visibility:public"],
    deps = [
        ":interface",
        "//cc/components/posix:error",
        "//cc/components/posix/proactor/sync",
    ],
)
