load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "logger",
    srcs = [
        "logger.cc",
    ],
    hdrs = [
        "logger.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":minimal",
        "//cc/bzd/core/logger/backend",
        "//cc/bzd/utility/format",
    ],
)

cc_library(
    name = "minimal",
    hdrs = [
        "minimal.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:string_view",
    ],
)
