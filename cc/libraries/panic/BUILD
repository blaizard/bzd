load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "panic",
    visibility = ["//visibility:public"],
    deps = select({
        "//cc/libraries/panic/throw:backend": ["//cc/libraries/panic/throw"],
        "//cc/libraries/panic/trap:backend": ["//cc/libraries/panic/trap"],
        "//cc/libraries/panic/exit:backend": ["//cc/libraries/panic/exit"],
        "//conditions:default": ["//cc/libraries/panic/exit"],
    }),
)
