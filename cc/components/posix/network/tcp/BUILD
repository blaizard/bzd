load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "client",
    hdrs = [
        "client.hh",
    ],
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
    visibility = ["//visibility:public"],
    deps = [
        "//cc/components/posix/network:socket",
    ],
)
