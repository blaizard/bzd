load("@rules_cc//cc:defs.bzl", "cc_library")
load("//cc/bdl:cc.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "platform",
    visibility = ["//visibility:public"],
    deps = [
        ":atomic",
        ":compiler",
        ":panic",
        ":types",
    ],
)

# ---- Individual items ----

cc_library(
    name = "compiler",
    hdrs = [
        "compiler.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "types",
    hdrs = [
        "types.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "panic",
    hdrs = [
        "panic.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:function_ref",
        "//cc/bzd/core:channel",
    ],
)

cc_library(
    name = "atomic",
    hdrs = [
        "atomic.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:named_type",
    ],
)
