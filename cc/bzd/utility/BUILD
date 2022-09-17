load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "utility",
    visibility = ["//visibility:public"],
    deps = [
        ":advance",
        ":align_down",
        ":align_up",
        ":apply",
        ":begin",
        ":constexpr_for",
        ":container_of",
        ":distance",
        ":end",
        ":forward",
        ":ignore",
        ":in_place",
        ":max",
        ":min",
        ":move",
        ":numeric_limits",
        ":offset_of",
        ":ownership",
        ":ratio",
        ":scope_guard",
        ":signal",
        ":singleton",
        ":size",
        ":source_location",
        ":swap",
        ":to_underlying",
        ":withdraw",
        "//cc/bzd/utility/bit",
        "//cc/bzd/utility/format",
        "//cc/bzd/utility/format:integral",
        "//cc/bzd/utility/format:stream",
        "//cc/bzd/utility/random",
        "//cc/bzd/utility/synchronization",
    ],
)

# ---- Individual items ----

cc_library(
    name = "advance",
    hdrs = [
        "advance.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:iterator",
    ],
)

cc_library(
    name = "align_down",
    hdrs = [
        "align_down.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "align_up",
    hdrs = [
        "align_up.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "apply",
    hdrs = [
        "apply.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":forward",
        "//cc/bzd/meta:range",
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:remove_reference",
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
    name = "constexpr_for",
    hdrs = [
        "constexpr_for.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:decay",
        "//cc/bzd/type_traits:integral_constant",
        "//cc/bzd/type_traits:invoke_result",
        "//cc/bzd/type_traits:is_same",
    ],
)

cc_library(
    name = "container_of",
    hdrs = [
        "container_of.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":offset_of",
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
    name = "forward",
    hdrs = [
        "forward.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:is_lvalue_reference",
        "//cc/bzd/type_traits:remove_reference",
    ],
)

cc_library(
    name = "ignore",
    hdrs = [
        "ignore.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "in_place",
    hdrs = [
        "in_place.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "max",
    hdrs = [
        "max.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "min",
    hdrs = [
        "min.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "move",
    hdrs = [
        "move.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:remove_reference",
    ],
)

cc_library(
    name = "numeric_limits",
    hdrs = [
        "numeric_limits.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "offset_of",
    hdrs = [
        "offset_of.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "ownership",
    hdrs = [
        "ownership.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/core/assert",
    ],
)

cc_library(
    name = "ratio",
    hdrs = [
        "ratio.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "scope_guard",
    hdrs = [
        "scope_guard.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":move",
        "//cc/bzd/container:optional",
    ],
)

cc_library(
    name = "signal",
    hdrs = [
        "signal.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/algorithm:copy_n",
        "//cc/bzd/algorithm:reverse",
        "//cc/bzd/container:span",
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "singleton",
    hdrs = [
        "singleton.hh",
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
    name = "source_location",
    hdrs = [
        "source_location.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "swap",
    hdrs = [
        "swap.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":move",
    ],
)

cc_library(
    name = "to_underlying",
    hdrs = [
        "to_underlying.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:underlying_type",
    ],
)

cc_library(
    name = "withdraw",
    hdrs = [
        "withdraw.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:iterator",
    ],
)
