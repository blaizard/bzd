load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "panic",
    visibility = ["//visibility:public"],
    deps = select({
        "//cc/bzd/lib/panic/throw:backend": ["//cc/bzd/lib/panic/throw"],
        "//cc/bzd/lib/panic/trap:backend": ["//cc/bzd/lib/panic/trap"],
        "//cc/bzd/lib/panic/exit:backend": ["//cc/bzd/lib/panic/exit"],
        "//conditions:default": ["//cc/bzd/lib/panic/exit"],
    }),
)
