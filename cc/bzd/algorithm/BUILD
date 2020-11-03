load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "algorithm",
    visibility = ["//visibility:public"],
    deps = [
        ":copy",
        ":copy_n",
        ":reverse",
    ],
)

# ---- Individual items ----

cc_library(
    name = "copy",
    hdrs = [
        "copy.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "copy_n",
    hdrs = [
        "copy_n.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "reverse",
    hdrs = [
        "reverse.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/utility:swap",
    ],
)
