load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "async",
    srcs = [
        "async.cc",
        "cancellation.cc",
        "error.cc",
        "generator.cc",
    ],
    deps = [
        "//cc/bzd/core/async",
        "//cc/bzd/test",
        "//cc/bzd/test/types:move_only",
        "//cc/bzd/utility:scope_guard",
    ],
)

bzd_cc_test(
    name = "multithread",
    timeout = "long",
    srcs = [
        "multithread.cc",
        "stack_overflow.cc",
        "suspend.cc",
    ],
    tags = ["stress"],
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
