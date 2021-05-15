load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "posix",
    srcs = ["bootstrap.cc"],
    tags = ["manual"],
    visibility = ["//visibility:public"],
    linkopts = [
        "-ldl",
    ],
    deps = [
        "//cc/bzd/container:array",
        "//cc/bzd/container:optional",
        "//cc/bzd/container:string_view",
        "//cc/bzd/platform:types",
        "//cc/bzd/utility:ignore",
        "//cc/bzd/utility:singleton",
        "//cc/bzd/utility/format",
    ],
    alwayslink = True,
)
