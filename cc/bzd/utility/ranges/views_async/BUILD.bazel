load("@rules_cc//cc:defs.bzl", "cc_library")
load("//cc/bdl:cc.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "views_async",
    visibility = ["//visibility:public"],
    deps = [
        ":drop",
        ":join",
    ],
)

# ---- Individual items ----

cc_library(
    name = "drop",
    hdrs = [
        "drop.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/core/async",
        "//cc/bzd/utility/iterators:advance_async",
        "//cc/bzd/utility/ranges/views:drop",
    ],
)

cc_library(
    name = "join",
    hdrs = [
        "join.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/core/async",
        "//cc/bzd/utility/iterators:container_of_iterables_async",
        "//cc/bzd/utility/ranges/views:join",
    ],
)
