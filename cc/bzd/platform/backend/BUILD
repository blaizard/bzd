load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "backend",
    srcs = [
        "backend.cc",
    ],
    visibility = ["//cc/bzd/platform:__pkg__"],
    deps = [
        "//cc/bzd/platform:clock",
        "//cc/bzd/platform:stream",
        "//cc/targets",
    ],
)

alias(
    name = "atomic",
    actual = "//cc/targets:atomic",
    visibility = ["//cc/bzd/platform:__pkg__"],
)
