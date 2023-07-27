load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "views_async",
    visibility = ["//visibility:public"],
    deps = [
        ":drop",
    ],
)

# ---- Individual items ----

cc_library(
    name = "drop",
    hdrs = [
        "drop.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container/ranges/views:drop",
        "//cc/bzd/core/async",
        "//cc/bzd/utility:advance_async",
    ],
)
