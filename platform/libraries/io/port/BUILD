load("@//tools/bzd:defs.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "port",
    hdrs = select({
        "@esp32_xtensa_lx6_gcc//:target": ["port_esp32_xtensa_lx6.h"],
        "//conditions:default": [],
    }),
    includes = select({
        "@esp32_xtensa_lx6_gcc//:target": ["libraries/io/port/port_esp32_xtensa_lx6.h"],
        "//conditions:default": [],
    }),
    interfaces = select({
        "@esp32_xtensa_lx6_gcc//:target": {"bzd::Port": "bzd::io::PortEsp32XtensaLx6"},
        "//conditions:default": {"bzd::Port": "*"},
    }),
    visibility = ["//visibility:public"],
    deps = [
        "//platform:bzd_core",
        "//platform:sdk",
    ],
)
