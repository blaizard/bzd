load("@rules_python//python:defs.bzl", "py_binary")
load("@python_deps//:requirements.bzl", "requirement")

package(default_visibility = ["//visibility:public"])

py_binary(
    name = "esptool",
    srcs = [
        "esptool.py",
    ],
    main = "esptool.py",
    visibility = ["//visibility:public"],
    deps = [
        requirement("pyserial")
    ]
)
