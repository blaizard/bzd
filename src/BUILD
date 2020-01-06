load("@bazel_skylib//lib:selects.bzl", "selects")

cc_library(
    name = "bzd",
    visibility = ["//visibility:public"],
    deps = [
        "//include:bzd",
        "//src/bzd/log",
        "//src/bzd/panic",
    ] + selects.with_or({
        (
            "//tools/bazel.build/config:x86_64",
            "//tools/bazel.build/config/auto:x86_64",
        ): ["//src/bzd/platforms/x86_64_cdecl"],
        "//conditions:default": [],
    }),
)
