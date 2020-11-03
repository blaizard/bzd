load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "stack",
    hdrs = [
        "stack.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "task",
    hdrs = [
        "task.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":stack",
        "//cc/bzd/platform:types",
        #"//cc/bzd/platform:system",
    ],
)
