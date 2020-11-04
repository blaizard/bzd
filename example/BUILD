load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "example",
    srcs = glob([
        "*.h",
        "*.cc",
    ]),
    tags = ["manual"],
    deps = [
        "//cc/bzd",
    ],
)
