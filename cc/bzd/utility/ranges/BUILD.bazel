load("@rules_cc//cc:defs.bzl", "cc_library")
load("//cc/bdl:cc.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "ranges",
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/utility/ranges/views",
        "//cc/bzd/utility/ranges/views_async",
    ],
)

cc_library(
    name = "associate_scope",
    hdrs = [
        "associate_scope.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":begin",
        ":end",
        ":view_interface",
        "//cc/bzd/container:tuple",
        "//cc/bzd/type_traits:is_rvalue_reference",
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
    name = "in_out_result",
    hdrs = [
        "in_out_result.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "size",
    hdrs = [
        "size.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "stream",
    hdrs = [
        "stream.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":view_interface",
        "//cc/bzd/container:wrapper",
        "//cc/bzd/type_traits:is_same_class",
        "//cc/bzd/type_traits:is_same_template",
        "//cc/bzd/type_traits:sentinel_for",
        "//cc/bzd/utility/iterators:advance",
        "//cc/bzd/utility/iterators:distance",
        "//cc/bzd/utility/iterators:input_or_output_reference",
        "//cc/bzd/utility/ranges/views:adaptor",
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
        "//cc/bzd/type_traits:is_base_of",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/type_traits:sized_sentinel_for",
    ],
)
