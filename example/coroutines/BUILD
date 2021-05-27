load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_binary", "cc_library")

bzd_cc_binary(
    name = "coroutines",
    srcs = [
        "main.cc",
    ],
    copts = [
        "-Wno-unused-value",
    ],
    tags = ["manual"],
    deps = [
        "//cc/bzd",
    ],
)
