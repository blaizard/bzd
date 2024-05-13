load("//cc/bdl:cc.bzl", "bzd_cc_test")

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
    name = "suspend",
    timeout = "moderate",
    srcs = [
        "suspend.cc",
    ],
    tags = ["stress"],
    target_compatible_with = select({
        "@esp32//:toolchain": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
    deps = [
        "//cc/bzd/core/async",
        "//cc/bzd/test:multithread",
        "//cc/bzd/test:sync_barrier",
    ],
)

bzd_cc_test(
    name = "stack_overflow",
    timeout = "moderate",
    srcs = [
        "stack_overflow.cc",
    ],
    tags = ["stress"],
    deps = [
        "//cc/bzd/core/async",
        "//cc/bzd/test",
    ],
)

bzd_cc_test(
    name = "multithread",
    timeout = "eternal",
    srcs = [
        "multithread.cc",
    ],
    tags = ["stress"],
    target_compatible_with = select({
        "@esp32//:toolchain": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
    deps = [
        "//cc/bzd/core/async",
        "//cc/bzd/test:multithread",
    ],
)
