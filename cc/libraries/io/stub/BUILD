load("@//tools/bzd:defs.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "stub",
    hdrs = [
        "stub.h",
    ],
    includes = [
        "libraries/io/stub/stub.h",
    ],
    interfaces = {
        "bzd::io::Stub": "*",
    },
    visibility = ["//visibility:public"],
    deps = [
        "//cc:bzd_core",
    ],
)
