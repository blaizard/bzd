load("@rules_python//python:defs.bzl", "py_binary", "py_library")
load("@bzd_toolchain_cc_pip//:requirements.bzl", "requirement")

py_binary(
    name = "esptool_binary",
    main = "esptool.py",
    srcs = [
        "esptool.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        requirement("pyserial")
    ],
    data = glob(["esptool/**"]),
)
