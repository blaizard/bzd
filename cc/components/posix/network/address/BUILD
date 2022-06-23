load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "address",
    srcs = [
        "address.cc",
        "ipv4.cc",
    ],
    hdrs = [
        "address.hh",
        "family.hh",
        "ipv4.hh",
    ],
    tags = ["manual"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//cc/bzd/container:result",
        "//cc/bzd/container:variant",
        "//cc/bzd/core:error",
        "//cc/bzd/utility:to_underlying",
        "//cc/components/posix:error",
        "//cc/components/posix/network:interface",
    ],
)
