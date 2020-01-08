load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "panic",
    visibility = ["//visibility:public"],
    deps = select({
        "//platform/libraries/panic/throw:backend": ["//platform/libraries/panic/throw"],
        "//platform/libraries/panic/trap:backend": ["//platform/libraries/panic/trap"],
        "//platform/libraries/panic/exit:backend": ["//platform/libraries/panic/exit"],
        "//conditions:default": ["//platform/libraries/panic/exit"],
    }),
)
