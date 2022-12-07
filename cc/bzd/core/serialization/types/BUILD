load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "integral",
    hdrs = [
        "integral.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/algorithm:byte_copy",
        "//cc/bzd/container:span",
        "//cc/bzd/core/serialization",
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:is_integral",
        "//cc/bzd/type_traits:is_same",
        "//cc/bzd/type_traits:remove_cvref",
    ],
)

cc_library(
    name = "floating_point",
    hdrs = [
        "floating_point.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/algorithm:byte_copy",
        "//cc/bzd/container:span",
        "//cc/bzd/core/serialization",
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:is_floating_point",
        "//cc/bzd/type_traits:is_same",
        "//cc/bzd/type_traits:remove_cvref",
        "//cc/bzd/utility:numeric_limits",
    ],
)
