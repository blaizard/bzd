load("@rules_cc//cc:defs.bzl", "cc_library")
load("//cc/bdl:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "tests",
    srcs = [
        "client.cc",
    ],
    deps = [
        ":mock",
        "//cc:bzd",
        "//cc/bdl/generator/impl/adapter:context",
        "//cc/bzd/test",
    ],
)

cc_library(
    name = "mock",
    testonly = True,
    hdrs = [
        "mock.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/test",
        "//cc/components/generic/network/tcp:client",
    ],
)
