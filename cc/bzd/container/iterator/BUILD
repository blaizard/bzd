load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "appender",
    hdrs = [
        "appender.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/utility:move",
    ],
)

cc_library(
    name = "container_of_iterables",
    hdrs = [
        "container_of_iterables.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:optional",
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:iterator",
    ],
)

cc_library(
    name = "contiguous",
    hdrs = [
        "contiguous.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":random_access",
    ],
)

cc_library(
    name = "bidirectional",
    hdrs = [
        "bidirectional.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":forward",
    ],
)

cc_library(
    name = "forward",
    hdrs = [
        "forward.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:conditional",
        "//cc/bzd/type_traits:is_same",
        "//cc/bzd/type_traits:iterator",
    ],
)

cc_library(
    name = "random_access",
    hdrs = [
        "random_access.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":bidirectional",
    ],
)

cc_library(
    name = "tuple",
    hdrs = [
        "tuple.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:variant",
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:add_reference",
        "//cc/bzd/type_traits:iterator",
    ],
)
