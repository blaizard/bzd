load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "random",
    visibility = ["//visibility:public"],
    deps = [
        ":linear_congruential_engine",
        ":uniform_int_distribution",
        ":xorwow_engine",
    ],
)

# ---- Individual items ----

cc_library(
    name = "linear_congruential_engine",
    hdrs = [
        "linear_congruential_engine.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "uniform_int_distribution",
    hdrs = [
        "uniform_int_distribution.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:is_convertible",
        "//cc/bzd/utility:numeric_limits",
    ],
)

cc_library(
    name = "xorwow_engine",
    hdrs = [
        "xorwow_engine.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
    ],
)
