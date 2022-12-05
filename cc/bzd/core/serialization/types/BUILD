load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "integral",
    hdrs = [
        "integral.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/core/serialization",
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:is_integral",
        "//cc/bzd/type_traits:is_same",
        "//cc/bzd/type_traits:remove_cvref",
    ],
)
