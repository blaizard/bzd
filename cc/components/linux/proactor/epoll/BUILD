load("@bzd_bdl//:defs.bzl", "bdl_library")
load("@rules_cc//cc:defs.bzl", "cc_library")

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
    target_compatible_with = [
        "@bzd_platforms//al:linux",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":interface",
        "//cc/components/posix:error",
        "//cc/components/posix/proactor/sync",
    ],
)
