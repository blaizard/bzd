load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "meta",
    visibility = ["//visibility:public"],
    deps = [
        ":choose_nth",
        ":contains",
        ":find",
        ":find_conditional",
        ":range",
        ":type",
        ":type_list",
        ":union",
    ],
)

# ---- Individual items ----

cc_library(
    name = "choose_nth",
    hdrs = [
        "choose_nth.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":type",
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "contains",
    hdrs = [
        "contains.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:is_same",
    ],
)

cc_library(
    name = "find_conditional",
    hdrs = [
        "find_conditional.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "find",
    hdrs = [
        "find.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":find_conditional",
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:is_same",
    ],
)

cc_library(
    name = "range",
    hdrs = [
        "range.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "type_list",
    hdrs = [
        "type_list.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":choose_nth",
        ":contains",
        ":find",
        ":find_conditional",
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "type",
    hdrs = [
        "type.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "union",
    hdrs = [
        "union.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:conditional",
        "//cc/bzd/type_traits:enable_if",
        "//cc/bzd/type_traits:first_type",
        "//cc/bzd/type_traits:is_same",
        "//cc/bzd/type_traits:is_trivially_destructible",
        "//cc/bzd/type_traits:remove_reference",
        "//cc/bzd/utility:forward",
        "//cc/bzd/utility:in_place",
    ],
)
