load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "non_owning",
    hdrs = [
        "non_owning.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:enable_if",
        "//cc/bzd/type_traits:is_const",
        "//cc/bzd/type_traits:remove_const",
    ],
)

cc_library(
    name = "resizeable",
    hdrs = [
        "resizeable.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "fixed",
    hdrs = [
        "fixed.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
    ],
)
