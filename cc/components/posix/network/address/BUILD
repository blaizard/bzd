load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "address",
    srcs = [
        "address.cc",
    ],
    hdrs = [
        "address.hh",
        "family.hh",
        "protocol.hh",
        "socket_type.hh",
    ],
    tags = ["manual"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//cc/bzd/algorithm:copy_n",
        "//cc/bzd/container:result",
        "//cc/bzd/container:variant",
        "//cc/bzd/container/iterator:forward",
        "//cc/bzd/core:error",
        "//cc/bzd/meta:string_literal",
        "//cc/bzd/utility:to_underlying",
        "//cc/components/posix:error",
        "//cc/components/posix/network:interface",
    ],
)
