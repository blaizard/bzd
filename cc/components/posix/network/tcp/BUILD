load("@bzd_bdl//:defs.bzl", "bdl_library")
load("@rules_cc//cc:defs.bzl", "cc_library")

bdl_library(
    name = "tcp",
    srcs = [
        "interface.bdl",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//cc/components/posix/proactor",
        "//interfaces:tcp",
    ],
)

cc_library(
    name = "client",
    hdrs = [
        "client.hh",
    ],
    tags = ["manual"],
    visibility = ["//visibility:public"],
    deps = [
        ":tcp",
        "//cc/components/posix/network:socket",
        "//cc/components/posix/proactor",
    ],
)

cc_library(
    name = "server",
    hdrs = [
        "server.hh",
    ],
    tags = ["manual"],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/components/posix/network:socket",
    ],
)
