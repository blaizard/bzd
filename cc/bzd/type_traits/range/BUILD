load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "range",
    visibility = ["//visibility:public"],
    deps = [
        ":traits",
    ],
)

# ---- Individual items ----

cc_library(
    name = "traits",
    hdrs = [
        "traits.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:declval",
        "//cc/bzd/type_traits/iterator:begin",
        "//cc/bzd/type_traits/iterator:end",
        "//cc/bzd/type_traits/iterator:traits",
    ],
)
