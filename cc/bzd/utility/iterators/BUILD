load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "advance",
    hdrs = [
        "advance.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/math:abs",
        "//cc/bzd/type_traits:assignable_from",
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/type_traits:sentinel_for",
        "//cc/bzd/type_traits:sized_sentinel_for",
        "//cc/bzd/utility:move",
    ],
)

cc_library(
    name = "advance_async",
    hdrs = [
        "advance_async.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":advance",
        "//cc/bzd/core/async",
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
    name = "container_of_iterables_async",
    hdrs = [
        "container_of_iterables_async.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":container_of_iterables",
        "//cc/bzd/core/async",
    ],
)

cc_library(
    name = "container_of_iterables",
    hdrs = [
        "container_of_iterables.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":sentinel",
        "//cc/bzd/container:optional",
        "//cc/bzd/core/async:forward",
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/type_traits:sentinel_for",
    ],
)

cc_library(
    name = "distance",
    hdrs = [
        "distance.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/type_traits:sentinel_for",
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
        "//cc/bzd/type_traits:remove_reference",
    ],
)

cc_library(
    name = "input_or_output_reference",
    hdrs = [
        "input_or_output_reference.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:reference_wrapper",
        "//cc/bzd/type_traits:is_same",
        "//cc/bzd/type_traits:iterator",
    ],
)

cc_library(
    name = "next",
    hdrs = [
        "next.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":advance",
    ],
)

cc_library(
    name = "prev",
    hdrs = [
        "prev.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":advance",
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
    name = "reverse",
    hdrs = [
        "reverse.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/utility/iterators:prev",
    ],
)

cc_library(
    name = "sentinel",
    hdrs = [
        "sentinel.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
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
