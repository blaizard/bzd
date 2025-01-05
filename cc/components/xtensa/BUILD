load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "error",
    hdrs = [
        "error.hh",
    ],
    target_compatible_with = select({
        "@bzd_platforms//isa:xtensa_lx6": [],
        "@bzd_platforms//isa:xtensa_lx7": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//cc/bzd/container:result",
        "//cc/bzd/core:error",
    ],
)
