load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "associate_scope",
    hdrs = [
        "associate_scope.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":view_interface",
        "//cc/bzd/container:tuple",
        "//cc/bzd/type_traits:is_rvalue_reference",
        "//cc/bzd/utility:begin",
        "//cc/bzd/utility:end",
    ],
)

cc_library(
    name = "in_out_result",
    hdrs = [
        "in_out_result.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "stream",
    hdrs = [
        "stream.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:wrapper",
        "//cc/bzd/container/iterator:input_or_output_reference",
        ":view_interface",
        "//cc/bzd/type_traits:is_same_class",
        "//cc/bzd/type_traits:is_same_template",
        "//cc/bzd/type_traits:sentinel_for",
        "//cc/bzd/utility:advance",
        "//cc/bzd/utility:distance",
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
        "//cc/bzd/type_traits:range",
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
        "//cc/bzd/type_traits:conditional",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/type_traits:sized_sentinel_for",
    ],
)
