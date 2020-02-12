load("@//tools/bzd:defs.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "digital",
    srcs = select({
        "@esp32_xtensa_lx6_gcc//:target": ["digital_esp32_xtensa_lx6.cpp"],
        "//conditions:default": [],
    }),
    hdrs = [
        "digital.h",
    ],
    includes = [
        "libraries/io/digital/digital.h",
    ],
    interfaces = {
        "bzd::io::DigitalOutput": "*",
    },
    visibility = ["//visibility:public"],
    deps = [
        "//platform:bzd_core",
        "//platform:sdk",
        "//platform/libraries/io/port",
    ],
)
