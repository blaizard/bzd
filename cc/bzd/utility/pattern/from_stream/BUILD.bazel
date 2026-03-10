load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "base",
    hdrs = [
        "base.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/core/async",
        "//cc/bzd/meta:always_false",
        "//cc/bzd/utility/ranges:stream",
    ],
)

cc_library(
    name = "integral",
    hdrs = [
        "integral.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":base",
        "//cc/bzd/type_traits:is_integral",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/utility/pattern/from_string:integral",
    ],
)

cc_library(
    name = "range_of_ranges",
    hdrs = [
        "range_of_ranges.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":base",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/utility/pattern/from_string:range_of_ranges",
    ],
)

cc_library(
    name = "range",
    hdrs = [
        "range.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":base",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/utility/pattern/from_string:range",
        "//cc/bzd/utility/regexp:async",
    ],
)

cc_library(
    name = "pattern",
    hdrs = [
        "pattern.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":base",
        "//cc/bzd/container:string_view",
        "//cc/bzd/utility/pattern:async",
        "//cc/bzd/utility/regexp:async",
    ],
)
