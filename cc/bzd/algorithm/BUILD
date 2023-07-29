load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "algorithm",
    visibility = ["//visibility:public"],
    deps = [
        ":all_of",
        ":any_of",
        ":binary_search",
        ":byte_copy",
        ":copy",
        ":copy_backward",
        ":copy_n",
        ":equal",
        ":equal_range",
        ":fill",
        ":fill_n",
        ":find",
        ":find_if",
        ":find_if_not",
        ":lexicographical_compare",
        ":lower_bound",
        ":move",
        ":move_backward",
        ":none_of",
        ":reverse",
        ":rsearch",
        ":search",
        ":sort",
        ":starts_with_any_of",
        ":upper_bound",
    ],
)

# ---- Individual items ----

cc_library(
    name = "all_of",
    hdrs = [
        "all_of.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":find_if_not",
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/type_traits:predicate",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/type_traits:sentinel_for",
    ],
)

cc_library(
    name = "any_of",
    hdrs = [
        "any_of.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":find_if_not",
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/type_traits:predicate",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/type_traits:sentinel_for",
    ],
)

cc_library(
    name = "binary_search",
    hdrs = [
        "binary_search.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":lower_bound",
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/type_traits:predicate",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/type_traits:sentinel_for",
        "//cc/bzd/utility:forward",
    ],
)

cc_library(
    name = "byte_copy",
    hdrs = [
        "byte_copy.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:is_trivially_copyable",
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/type_traits:sentinel_for",
        "//cc/bzd/utility:min",
        "//cc/bzd/utility/ranges:in_out_result",
    ],
)

cc_library(
    name = "copy",
    hdrs = [
        "copy.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/type_traits:sentinel_for",
        "//cc/bzd/utility:min",
        "//cc/bzd/utility/ranges:in_out_result",
    ],
)

cc_library(
    name = "copy_backward",
    hdrs = [
        "copy_backward.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/type_traits:sentinel_for",
    ],
)

cc_library(
    name = "copy_n",
    hdrs = [
        "copy_n.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/type_traits:range",
    ],
)

cc_library(
    name = "equal_range",
    hdrs = [
        "equal_range.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":lower_bound",
        ":upper_bound",
        "//cc/bzd/container:tuple",
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/type_traits:predicate",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/type_traits:sentinel_for",
        "//cc/bzd/utility:forward",
        "//cc/bzd/utility/comparison",
    ],
)

cc_library(
    name = "equal",
    hdrs = [
        "equal.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:is_same",
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/type_traits:predicate",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/type_traits:sentinel_for",
        "//cc/bzd/utility:forward",
        "//cc/bzd/utility/comparison",
    ],
)

cc_library(
    name = "fill",
    hdrs = [
        "fill.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/type_traits:sentinel_for",
        "//cc/bzd/utility:forward",
    ],
)

cc_library(
    name = "fill_n",
    hdrs = [
        "fill_n.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/utility:forward",
    ],
)

cc_library(
    name = "find",
    hdrs = [
        "find.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":find_if",
        "//cc/bzd/type_traits:sentinel_for",
    ],
)

cc_library(
    name = "find_if",
    hdrs = [
        "find_if.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/type_traits:predicate",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/type_traits:sentinel_for",
        "//cc/bzd/utility:forward",
    ],
)

cc_library(
    name = "find_if_not",
    hdrs = [
        "find_if_not.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":find_if",
        "//cc/bzd/type_traits:predicate",
        "//cc/bzd/type_traits:sentinel_for",
    ],
)

cc_library(
    name = "lexicographical_compare",
    hdrs = [
        "lexicographical_compare.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:is_same",
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/type_traits:predicate",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/type_traits:sentinel_for",
        "//cc/bzd/utility:forward",
        "//cc/bzd/utility:swap",
        "//cc/bzd/utility/comparison",
    ],
)

cc_library(
    name = "lower_bound",
    hdrs = [
        "lower_bound.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/type_traits:predicate",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/type_traits:sentinel_for",
        "//cc/bzd/utility:forward",
        "//cc/bzd/utility/comparison",
        "//cc/bzd/utility/iterators:advance",
        "//cc/bzd/utility/iterators:distance",
    ],
)

cc_library(
    name = "move",
    hdrs = [
        "move.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/type_traits:sentinel_for",
        "//cc/bzd/utility:move",
    ],
)

cc_library(
    name = "move_backward",
    hdrs = [
        "move_backward.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/type_traits:sentinel_for",
        "//cc/bzd/utility:move",
    ],
)

cc_library(
    name = "none_of",
    hdrs = [
        "none_of.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":find_if",
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/type_traits:predicate",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/type_traits:sentinel_for",
        "//cc/bzd/utility:forward",
    ],
)

cc_library(
    name = "reverse",
    hdrs = [
        "reverse.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/type_traits:sentinel_for",
        "//cc/bzd/utility:swap",
    ],
)

cc_library(
    name = "rsearch",
    hdrs = [
        "rsearch.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":search",
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/type_traits:predicate",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/type_traits:sentinel_for",
        "//cc/bzd/utility:forward",
        "//cc/bzd/utility:withdraw",
        "//cc/bzd/utility/comparison",
    ],
)

cc_library(
    name = "search",
    hdrs = [
        "search.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/type_traits:predicate",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/type_traits:sentinel_for",
        "//cc/bzd/utility:forward",
        "//cc/bzd/utility/comparison",
    ],
)

cc_library(
    name = "sort",
    hdrs = [
        "sort.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/type_traits:predicate",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/type_traits:remove_cvref",
        "//cc/bzd/type_traits:sentinel_for",
        "//cc/bzd/utility:forward",
        "//cc/bzd/utility:swap",
        "//cc/bzd/utility/comparison",
        "//cc/bzd/utility/iterators:distance",
    ],
)

cc_library(
    name = "starts_with_any_of",
    hdrs = [
        "starts_with_any_of.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":lower_bound",
        ":upper_bound",
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/type_traits:sentinel_for",
        "//cc/bzd/utility/ranges:begin",
        "//cc/bzd/utility/ranges:end",
        "//cc/bzd/utility/ranges:size",
    ],
)

cc_library(
    name = "upper_bound",
    hdrs = [
        "upper_bound.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/type_traits:predicate",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/type_traits:sentinel_for",
        "//cc/bzd/utility:forward",
        "//cc/bzd/utility/comparison",
        "//cc/bzd/utility/iterators:advance",
        "//cc/bzd/utility/iterators:distance",
    ],
)
