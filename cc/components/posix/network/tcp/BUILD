load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "client",
    hdrs = [
        "client.hh",
    ],
    tags = ["manual"],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/components/posix/network:socket",
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
