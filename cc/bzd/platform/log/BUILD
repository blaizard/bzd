load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "log",
    visibility = ["//visibility:public"],
    deps = select({
        "//cc/bzd/platform/log/stdout:backend": ["//cc/bzd/platform/log/stdout"],
        "//conditions:default": ["//cc/bzd/platform/log/stdout"],
    }),
)
