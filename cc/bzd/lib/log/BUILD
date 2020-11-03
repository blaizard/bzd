load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "log",
    visibility = ["//visibility:public"],
    deps = select({
        "//cc/bzd/lib/log/stdout:backend": ["//cc/bzd/lib/log/stdout"],
        "//conditions:default": ["//cc/bzd/lib/log/stdout"],
    }),
)
