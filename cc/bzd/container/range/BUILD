load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "associate_scope",
    hdrs = [
        "associate_scope.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:tuple",
        "//cc/bzd/utility:begin",
        "//cc/bzd/utility:end",
    ],
)

cc_library(
    name = "subrange",
    hdrs = [
        "subrange.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":view_interface",
        "//cc/bzd/type_traits:sentinel_for",
    ],
)

cc_library(
    name = "view_interface",
    hdrs = [
        "view_interface.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:range",
        "//cc/bzd/type_traits:sized_sentinel_for",
    ],
)
