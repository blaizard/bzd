load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "support",
    hdrs = [
        "backend.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:array",
        "//cc/bzd/core/logger/backend",
        "//cc/bzd/platform:types",
    ],
)
