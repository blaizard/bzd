load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_binary")

bzd_cc_binary(
    name = "clock",
    hdrs = [
        "counter.hh",
    ],
    bdls = [
        "composition.bdl",
    ],
    deps = [
        "//cc/bzd/core:delay",
    ],
)
