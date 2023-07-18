load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "tests",
    srcs = [
        "http.cc",
    ],
    deps = [
        "//cc:bzd",
        "//cc/bzd/test",
        "//cc/components/generic/network/tcp:client",
        "//cc/libs/http",
        "//tools/bdl/generators/cc/adapter:context",
    ],
)
