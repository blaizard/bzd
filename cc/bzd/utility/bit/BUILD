load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "bit",
    hdrs = [
        "count_lsb_one.hh",
        "count_lsb_zero.hh",
        "count_msb_one.hh",
        "count_msb_zero.hh",
        "endian.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
    ],
)
