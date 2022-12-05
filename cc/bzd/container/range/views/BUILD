load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "views",
    visibility = ["//visibility:public"],
    deps = [
        ":adaptor",
        ":drop",
        ":reverse",
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
        "//cc/bzd/container/range:view_interface",
        "//cc/bzd/utility:advance",
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
        "//cc/bzd/container/iterator:reverse",
        "//cc/bzd/container/range:view_interface",
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
        "//cc/bzd/container/range:view_interface",
        "//cc/bzd/utility:advance",
    ],
)
