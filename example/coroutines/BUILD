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
    name = "eager",
    srcs = [
        "eager.cc",
    ],
    tags = ["manual"],
    deps = [
        "//cc/bzd",
    ],
)

cc_library(
    name = "async",
    srcs = [
        "coroutine.h",
        "promise.h",
        "async.h",
        "scheduler.h",
    ],
    hdrs = [
        "async.h"
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd",
    ],
)

cc_binary(
    name = "2",
    srcs = [
        "coroutines2.cc",
    ],
    tags = [
        "cc"
    ],
    target_compatible_with = select({
        "@linux_x86_64_gcc//:target": [
            "@linux_x86_64_gcc//:compiler",
        ],
        "@linux_x86_64_clang//:target": [
            "@linux_x86_64_clang//:compiler",
        ],
        "//conditions:default": ["@linux_x86_64_clang//:compiler",]
    }),
    copts = [
        "-Wno-unused-value",
    ],
    linkopts = [
        "-pthread",
    ],
    deps = [
        "//cc/bzd",
        ":async"
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
