load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "assert",
    srcs = [
        "assert.cc",
    ],
    hdrs = [
        "//cc/bzd/core:assert.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":minimal",
        "//cc/bzd/container:result",
        "//cc/bzd/container:string",
        "//cc/bzd/container:string_view",
        "//cc/bzd/core/logger",
        "//cc/bzd/core/panic",
    ],
)

cc_library(
    name = "minimal",
    hdrs = [
        "minimal.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/utility:source_location",
    ],
)
