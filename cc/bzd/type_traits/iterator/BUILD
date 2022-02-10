load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "iterator",
    visibility = ["//visibility:public"],
    deps = [
        ":advance",
        ":begin",
        ":distance",
        ":end",
        ":traits",
    ],
)

# ---- Individual items ----

cc_library(
    name = "advance",
    hdrs = [
        "advance.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":traits",
    ],
)

cc_library(
    name = "begin",
    hdrs = [
        "begin.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "distance",
    hdrs = [
        "distance.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":traits",
    ],
)

cc_library(
    name = "end",
    hdrs = [
        "end.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "traits",
    hdrs = [
        "traits.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:derived_from",
        "//cc/bzd/type_traits:false_type",
        "//cc/bzd/type_traits:is_base_of",
        "//cc/bzd/type_traits:is_pointer",
        "//cc/bzd/type_traits:remove_pointer",
        "//cc/bzd/type_traits:true_type",
        "//cc/bzd/type_traits:void_type",
        "//cc/bzd/utility:concept",
    ],
)
