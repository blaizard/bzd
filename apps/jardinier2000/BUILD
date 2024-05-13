load("@bzd_bdl//:defs.bzl", "bdl_library")
load("@rules_cc//cc:defs.bzl", "cc_library")
load("//cc/bdl:cc.bzl", "bzd_cc_binary")

bdl_library(
    name = "composition",
    srcs = [
        "composition.bdl",
    ],
    deps = [
        "//interfaces:timer",
    ],
)

cc_library(
    name = "lib",
    srcs = [
        "jardinier.cc",
    ],
    hdrs = [
        "jardinier.hh",
    ],
    target_compatible_with = select({
        "@esp32//:toolchain": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    deps = [
        ":composition",
        "//cc:bzd",
    ],
)

bzd_cc_binary(
    name = "jardinier2000",
    deps = [
        ":lib",
    ],
)
