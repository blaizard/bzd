load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "views",
    visibility = ["//visibility:public"],
    deps = [
        ":adaptor",
        ":drop",
    ],
)

# ---- Individual items ----

cc_library(
    name = "adaptor",
    hdrs = [
        "adaptor.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/utility:forward",
        "//cc/bzd/type_traits:range"
    ],
)

cc_library(
    name = "drop",
    hdrs = [
        "drop.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":adaptor",
        "//cc/bzd/container/range:view_interface",
        "//cc/bzd/utility:advance",
    ],
)
