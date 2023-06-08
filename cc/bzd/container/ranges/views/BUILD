load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "views",
    visibility = ["//visibility:public"],
    deps = [
        ":adaptor",
        ":all",
        ":drop",
        ":owning",
        ":reverse",
        ":stream",
        ":take",
        ":transform",
    ],
)

# ---- Individual items ----

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
        "//cc/bzd/container:wrapper",
        "//cc/bzd/container/ranges:view_interface",
        "//cc/bzd/type_traits:is_same_class",
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
        "//cc/bzd/container:wrapper",
        "//cc/bzd/container/ranges:view_interface",
        "//cc/bzd/utility:advance",
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
        "//cc/bzd/container/ranges:view_interface",
        "//cc/bzd/utility:move",
        "//cc/bzd/utility:size",
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
        "//cc/bzd/container:wrapper",
        "//cc/bzd/container/iterator:reverse",
        "//cc/bzd/container/ranges:view_interface",
        "//cc/bzd/type_traits:is_same_class",
    ],
)

cc_library(
    name = "stream",
    hdrs = [
        "stream.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":adaptor",
        "//cc/bzd/container:wrapper",
        "//cc/bzd/container/iterator:input_or_output_reference",
        "//cc/bzd/container/ranges:view_interface",
        "//cc/bzd/type_traits:is_same_class",
        "//cc/bzd/type_traits:is_same_template",
        "//cc/bzd/type_traits:sentinel_for",
        "//cc/bzd/utility:advance",
        "//cc/bzd/utility:distance",
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
        "//cc/bzd/container:wrapper",
        "//cc/bzd/container/ranges:view_interface",
        "//cc/bzd/utility:advance",
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
        "//cc/bzd/container:wrapper",
        "//cc/bzd/container/ranges:view_interface",
        "//cc/bzd/utility:advance",
    ],
)
