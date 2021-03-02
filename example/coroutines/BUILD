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

cc_binary(
    name = "2",
    srcs = [
        "coroutines2.cc",
    ],
    copts = [
        "-std=c++2a",
        "-stdlib=libc++",
        "-fcoroutines-ts",
    ],
    linkopts = [
        "-pthread",
    ],
    tags = ["manual"],
    deps = [
        "//cc/bzd",
    ],
)

cc_binary(
    name = "lewis",
    srcs = [
        "lewis.cc",
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
