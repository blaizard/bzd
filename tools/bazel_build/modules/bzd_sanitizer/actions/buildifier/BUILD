load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "buildifier",
    srcs = [
        "buildifier.py",
    ],
    args = [
        "--buildifier",
        "$(rootpath @buildifier_prebuilt//:buildifier)",
    ],
    data = [
        "@buildifier_prebuilt//:buildifier",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//python/bzd_sanitizer:worker",
        "@bzd_python//bzd/utils:run",
    ],
)
