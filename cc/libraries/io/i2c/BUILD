load("@rules_python//python:defs.bzl", "py_binary")
load("@//tools/bazel_build/rules:cc.bzl", "bzd_cc_library")

py_binary(
    name = "test",
    srcs = [
        "test.py",
    ],
    data = [
        ":i2c.bzl",
    ],
)

bzd_cc_library(
    name = "i2c",
    srcs = select({
        "@esp32_xtensa_lx6_gcc//:target": ["i2c_esp32_xtensa_lx6.cpp"],
        "//conditions:default": [],
    }),
    hdrs = [
        "i2c.h",
    ] + select({
        "@esp32_xtensa_lx6_gcc//:target": ["i2c_esp32_xtensa_lx6.h"],
        "//conditions:default": [],
    }),
    includes = select({
        "@esp32_xtensa_lx6_gcc//:target": ["libraries/io/i2c/i2c_esp32_xtensa_lx6.h"],
        "//conditions:default": [],
    }),
    interfaces = select({
        "@esp32_xtensa_lx6_gcc//:target": {"bzd::io::I2C": "bzd::io::impl::I2CEsp32XtensaLx6"},
        "//conditions:default": {"bzd::io::I2C": "*"},
    }),
    visibility = ["//visibility:public"],
    deps = [
        "//cc:bzd_core",
    ],
)
