load("@rules_cc//cc:defs.bzl", "cc_library")
load("//cc/bdl:cc.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "meta",
    visibility = ["//visibility:public"],
    deps = [
        ":always_false",
        ":choose_nth",
        ":contains",
        ":find",
        ":find_conditional",
        ":last",
        ":macro",
        ":range",
        ":string_literal",
        ":tuple",
        ":type",
        ":type_list",
        ":union",
    ],
)

# ---- Individual items ----

cc_library(
    name = "always_false",
    hdrs = [
        "always_false.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
    ],
)

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
    name = "last",
    hdrs = [
        "last.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":type",
    ],
)

cc_library(
    name = "macro",
    hdrs = [
        "macro.hh",
    ],
    visibility = ["//visibility:public"],
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
    name = "string_literal",
    hdrs = [
        "string_literal.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":range",
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
        ":choose_nth",
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
        "//cc/bzd/type_traits:is_same_class",
        "//cc/bzd/type_traits:is_trivially_destructible",
        "//cc/bzd/type_traits:remove_reference",
        "//cc/bzd/utility:forward",
        "//cc/bzd/utility:in_place",
    ],
)
