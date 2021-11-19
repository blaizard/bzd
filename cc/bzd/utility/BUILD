load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "utility",
    visibility = ["//visibility:public"],
    deps = [
        ":align_down",
        ":align_up",
        ":aligned_storage",
        ":bit",
        ":constexpr_for",
        ":container_of",
        ":forward",
        ":ignore",
        ":in_place",
        ":lock_guard",
        ":max",
        ":min",
        ":move",
        ":offset_of",
        ":ratio",
        ":scope_guard",
        ":signal",
        ":singleton",
        ":source_location",
        ":swap",
    ],
)

# ---- Individual items ----

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
    name = "aligned_storage",
    hdrs = [
        "aligned_storage.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "bit",
    hdrs = [
        "bit.hh",
    ],
    visibility = ["//visibility:public"],
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
    name = "lock_guard",
    hdrs = [
        "lock_guard.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":scope_guard",
        "//cc/bzd/core/async",
        "//cc/bzd/type_traits:invoke_result",
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
