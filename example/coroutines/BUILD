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

# For g++-10
# Run with bazel-real run example/coroutines:2 --config=docker -s --config=linux_x86_64_gcc --copt='-Wno-unused-value'
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
