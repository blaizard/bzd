load("@rules_cc//cc:defs.bzl", "cc_library")
load("//cc/bdl:cc.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "views",
    visibility = ["//visibility:public"],
    deps = [
        ":adaptor",
        ":all",
        ":drop",
        ":drop_last",
        ":filter",
        ":join",
        ":owning",
        ":ref",
        ":reverse",
        ":take",
        ":transform",
    ],
)

# ---- Individual items ----

cc_library(
    name = "adaptor_iterator",
    hdrs = [
        "adaptor_iterator.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/utility:move",
    ],
)

cc_library(
    name = "adaptor",
    hdrs = [
        "adaptor.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:range",
        "//cc/bzd/utility:forward",
        "//cc/bzd/utility:in_place",
        "//cc/bzd/utility:move",
    ],
)

cc_library(
    name = "all",
    hdrs = [
        "all.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":adaptor",
        ":owning",
        ":ref",
        "//cc/bzd/container:wrapper",
        "//cc/bzd/type_traits:conditional",
        "//cc/bzd/type_traits:is_lvalue_reference",
        "//cc/bzd/type_traits:is_same_class",
        "//cc/bzd/utility/ranges:view_interface",
    ],
)

cc_library(
    name = "drop_last",
    hdrs = [
        "drop_last.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":adaptor",
        ":all",
        "//cc/bzd/container:wrapper",
        "//cc/bzd/core/async:forward",
        "//cc/bzd/utility/iterators:advance",
        "//cc/bzd/utility/ranges:view_interface",
    ],
)

cc_library(
    name = "drop",
    hdrs = [
        "drop.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":adaptor",
        ":all",
        "//cc/bzd/container:wrapper",
        "//cc/bzd/core/async:forward",
        "//cc/bzd/utility/iterators:advance",
        "//cc/bzd/utility/ranges:view_interface",
    ],
)

cc_library(
    name = "filter",
    hdrs = [
        "filter.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":adaptor",
        ":adaptor_iterator",
        ":all",
        "//cc/bzd/algorithm:find_if",
        "//cc/bzd/container:wrapper",
        "//cc/bzd/type_traits:predicate",
        "//cc/bzd/utility/ranges:view_interface",
    ],
)

cc_library(
    name = "join",
    hdrs = [
        "join.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":adaptor",
        ":all",
        "//cc/bzd/container:wrapper",
        "//cc/bzd/type_traits:is_same_class",
        "//cc/bzd/utility:apply",
        "//cc/bzd/utility/iterators:container_of_iterables",
        "//cc/bzd/utility/ranges:view_interface",
    ],
)

cc_library(
    name = "owning",
    hdrs = [
        "owning.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":adaptor",
        "//cc/bzd/utility:move",
        "//cc/bzd/utility/ranges:size",
        "//cc/bzd/utility/ranges:view_interface",
    ],
)

cc_library(
    name = "ref",
    hdrs = [
        "ref.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":adaptor",
        "//cc/bzd/container:wrapper",
        "//cc/bzd/type_traits:is_same_class",
        "//cc/bzd/utility/ranges:view_interface",
    ],
)

cc_library(
    name = "reverse",
    hdrs = [
        "reverse.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":adaptor",
        ":all",
        "//cc/bzd/container:wrapper",
        "//cc/bzd/type_traits:is_same_class",
        "//cc/bzd/utility/iterators:reverse",
        "//cc/bzd/utility/ranges:view_interface",
    ],
)

cc_library(
    name = "take",
    hdrs = [
        "take.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":adaptor",
        ":all",
        "//cc/bzd/container:wrapper",
        "//cc/bzd/utility/iterators:advance",
        "//cc/bzd/utility/ranges:view_interface",
    ],
)

cc_library(
    name = "transform",
    hdrs = [
        "transform.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":adaptor",
        ":adaptor_iterator",
        ":all",
        "//cc/bzd/container:wrapper",
        "//cc/bzd/utility/iterators:advance",
        "//cc/bzd/utility/ranges:view_interface",
    ],
)
