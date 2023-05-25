load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "memory",
    visibility = ["//visibility:public"],
    deps = [
        ":non_null_ptr",
    ],
)

# ---- Individual items ----

cc_library(
    name = "non_null_ptr",
    hdrs = [
        "non_null_ptr.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/core/assert:minimal",
        "//cc/bzd/type_traits:is_pointer",
        "//cc/bzd/utility:forward",
        "//cc/bzd/utility:move",
    ],
)
