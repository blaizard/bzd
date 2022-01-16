load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "tests",
    srcs = [
        "format.cc",
        "integral.cc",
    ],
    deps = [
        "//cc/bzd/container:string_stream",
        "//cc/bzd/core/assert",
        "//cc/bzd/core/panic",
        "//cc/bzd/test",
        "//cc/bzd/utility/format",
    ],
)

bzd_cc_test(
    name = "tests_throw",
    srcs = [
        "format_throw.cc",
    ],
    target_compatible_with = [
        "@linux_x86_64_clang//:compiler",
    ],
    deps = [
        "//cc/bzd/container:string_view",
        "//cc/bzd/test",
        "//cc/bzd/utility/format",
    ],
)
