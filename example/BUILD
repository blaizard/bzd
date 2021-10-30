load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_binary")
load("//tools/bazel_build/rules:package.bzl", "bzd_package")

cc_library(
    name = "lib",
    srcs = glob([
        "*.hh",
        "*.cc",
    ]),
    linkopts = [
        "-pthread",
    ],
    tags = ["manual"],
    deps = [
        "//cc:bzd",
    ],
)

bzd_cc_binary(
    name = "example",
    tags = ["manual"],
    deps = [
        ":lib",
    ],
)

bzd_package(
    name = "package",
    include_metadata = True,
    tags = ["manual"],
    deps = {
        ":example": "example",
    },
)
