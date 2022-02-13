load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "async",
    srcs = [
        "async.cc",
    ],
    # TODO: re-enable with esp32 gcc 11
    target_compatible_with = select({
        "@esp32_xtensa_lx6_gcc//:compiler": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
    deps = [
        "//cc/bzd/core/async",
        "//cc/bzd/test",
    ],
)

bzd_cc_test(
    name = "multithread",
    srcs = [
        "multithread.cc",
    ],
    # TODO: re-enable with esp32 gcc 11
    target_compatible_with = select({
        "@esp32_xtensa_lx6_gcc//:compiler": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
    deps = [
        "//cc/bzd/core:delay",
        "//cc/bzd/core/async",
        "//cc/bzd/test",
    ],
)
