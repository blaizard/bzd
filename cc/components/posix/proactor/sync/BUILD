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
    name = "sync",
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
        "//cc/components/posix/network:socket_options",
    ],
)
