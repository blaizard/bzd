load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "panic",
    visibility = ["//visibility:public"],
    deps = select({
        "//cc/bzd/platform/panic/throw:backend": ["//cc/bzd/platform/panic/throw"],
        "//cc/bzd/platform/panic/trap:backend": ["//cc/bzd/platform/panic/trap"],
        "//cc/bzd/platform/panic/exit:backend": ["//cc/bzd/platform/panic/exit"],
        "//conditions:default": ["//cc/bzd/platform/panic/exit"],
    }),
)
