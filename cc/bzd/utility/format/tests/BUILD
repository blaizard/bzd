load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "tests",
    srcs = [
        "format.cc",
        "integral.cc",
    ],
    # TODO: re-enable with esp32 gcc 11
    target_compatible_with = select({
        "@esp32_xtensa_lx6_gcc//:compiler": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
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
    target_compatible_with = select({
        "@linux_x86_64_gcc//:compiler": [],
        "@linux_x86_64_clang//:compiler": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    deps = [
        "//cc/bzd/container:string_view",
        "//cc/bzd/test",
        "//cc/bzd/utility/format",
    ],
)
