load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_binary", "cc_library")
load("//tools/bazel_build/rules:package.bzl", "bzd_package")

cc_library(
    name = "lib",
    srcs = [
        "main.cc",
    ],
    deps = [
        "//cc/bzd",
    ],
)

bzd_cc_binary(
    name = "format",
    deps = [
        ":lib",
    ],
)

bzd_package(
    name = "package",
    include_metadata = True,
    tags = ["manual"],
    deps = {
        ":format": "format",
    },
)
