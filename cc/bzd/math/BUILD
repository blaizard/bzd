load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "math",
    visibility = ["//visibility:public"],
    deps = [
        ":abs",
        ":ceil",
        ":floor",
        ":trunc",
    ],
)

# ---- Individual items ----

cc_library(
    name = "abs",
    hdrs = [
        "abs.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:is_arithmetic",
    ],
)

cc_library(
    name = "ceil",
    hdrs = [
        "ceil.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":trunc",
    ],
)

cc_library(
    name = "floor",
    hdrs = [
        "floor.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":trunc",
    ],
)

cc_library(
    name = "trunc",
    hdrs = [
        "trunc.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:is_floating_point",
        "//cc/bzd/type_traits:is_integral",
        "//cc/bzd/utility:numeric_limits",
    ],
)
