load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "posix",
    srcs = ["bootstrap.cc"],
    tags = ["manual"],
    visibility = ["//visibility:public"],
    alwayslink = True,
    deps = [
        "//cc/bzd/container:array",
        "//cc/bzd/container:string_view",
        "//cc/bzd/container:optional",
        "//cc/bzd/utility:ignore",
        "//cc/bzd/platform:types",
        "//cc/bzd/utility:singleton",
    ]
)
