load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "coroutines",
    srcs = [
        "main.cc",
    ],
    copts = [
        "-std=c++2a",
        "-stdlib=libc++",
        "-fcoroutines-ts",
    ],
    tags = ["manual"],
    deps = [
        "//cc/bzd",
    ],
)
