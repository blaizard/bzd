load("@bazel_skylib//lib:selects.bzl", "selects")

cc_library(
    name = "bzd",
    visibility = ["//visibility:public"],
    deps = [
        "//include:bzd",
        "//src/bzd/log",
        "//src/bzd/panic",
    ] + selects.with_or({
        "//conditions:default": ["//src/bzd/platforms/x86_64_cdecl"],
    }),
)
