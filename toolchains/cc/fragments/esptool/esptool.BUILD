load("@rules_python//python:defs.bzl", "py_binary")
load("@python_deps//:requirements.bzl", "requirement")

py_binary(
    name = "wrapper",
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
