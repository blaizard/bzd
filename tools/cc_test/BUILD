load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "cc_test",
    srcs = [
        "test.cc",
    ],
    hdrs = [
        "test.hh",
        "test_internal.hh",
    ],
    include_prefix = "cc_test",
    includes = ["."],
    target_compatible_with = select({
        "@esp32_xtensa_lx6_gcc//:compiler": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
    visibility = ["//visibility:public"],
)
