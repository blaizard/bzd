load("@//tools/bzd:defs.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "digital",
    srcs = select({
        "@esp32_xtensa_lx6_gcc//:target": ["digital_esp32_xtensa_lx6.cpp"],
        "//conditions:default": [],
    }),
    hdrs = [
        "digital.h",
    ] + select({
        "@esp32_xtensa_lx6_gcc//:target": ["digital_esp32_xtensa_lx6.h"],
        "//conditions:default": [],
    }),
    includes = select({
        "@esp32_xtensa_lx6_gcc//:target": ["libraries/io/digital/digital_esp32_xtensa_lx6.h"],
        "//conditions:default": [],
    }),
    interfaces = select({
        "@esp32_xtensa_lx6_gcc//:target": {"bzd::io::DigitalOutput": "bzd::io::impl::DigitalOutputEsp32XtensaLx6"},
        "//conditions:default": {"bzd::io::DigitalOutput": "*"},
    }),
    visibility = ["//visibility:public"],
    deps = [
        "//platform:bzd_core",
        "//platform:sdk",
    ],
)
