load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "base",
    hdrs = [
        "base.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/core:channel",
        "//cc/bzd/core/async",
        "//cc/bzd/meta:always_false",
    ],
)

cc_library(
    name = "boolean",
    hdrs = [
        "boolean.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":base",
        "//cc/bzd/utility/pattern/to_string:boolean",
    ],
)

cc_library(
    name = "byte",
    hdrs = [
        "byte.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":base",
        "//cc/bzd/utility/pattern/to_string:byte",
    ],
)

cc_library(
    name = "enum",
    hdrs = [
        "enum.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":base",
        ":pattern",
        "//cc/bzd/utility/pattern/to_string:enum",
    ],
)

cc_library(
    name = "floating_point",
    hdrs = [
        "floating_point.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":base",
        "//cc/bzd/utility/pattern/to_string:floating_point",
    ],
)

cc_library(
    name = "generator",
    hdrs = [
        "generator.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":input_range",
    ],
)

cc_library(
    name = "input_range",
    hdrs = [
        "input_range.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":base",
        "//cc/bzd/container:string_view",
        "//cc/bzd/type_traits:is_convertible",
        "//cc/bzd/utility/pattern/to_string:input_range",
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
        "//cc/bzd/utility/pattern/to_string:integral",
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
    ],
)

cc_library(
    name = "pointer",
    hdrs = [
        "pointer.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":base",
        "//cc/bzd/utility/pattern/to_string:pointer",
    ],
)
