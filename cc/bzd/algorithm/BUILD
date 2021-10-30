load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "algorithm",
    visibility = ["//visibility:public"],
    deps = [
        ":copy",
        ":copy_n",
        ":fill",
        ":reverse",
    ],
)

# ---- Individual items ----

cc_library(
    name = "copy",
    hdrs = [
        "copy.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "copy_n",
    hdrs = [
        "copy_n.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "fill",
    hdrs = [
        "fill.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "reverse",
    hdrs = [
        "reverse.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/utility:swap",
    ],
)
