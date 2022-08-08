load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "stack_trace",
    srcs = ["stack_trace.cc"],
    linkopts = [
        "-ldl",
    ],
    tags = ["manual"],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:array",
        "//cc/bzd/container:optional",
        "//cc/bzd/container:string_view",
        "//cc/bzd/platform:types",
        "//cc/bzd/utility:ignore",
        "//cc/bzd/utility:singleton",
        "//cc/bzd/utility/format:stream",
        "//cc/components/posix:error",
    ],
    alwayslink = True,
)
