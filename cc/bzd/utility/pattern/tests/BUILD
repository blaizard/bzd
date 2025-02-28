load("//cc/bdl:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "pattern",
    srcs = [
        "pattern.cc",
        "wrong.cc",
    ],
    deps = [
        "//cc/bzd/container:string_stream",
        "//cc/bzd/core/assert",
        "//cc/bzd/test",
        "//cc/bzd/utility/pattern",
    ],
)

bzd_cc_test(
    name = "formatter",
    srcs = [
        "format.cc",
        "integral.cc",
        "enum.cc",
    ],
    deps = [
        "//cc/bzd/container:string_stream",
        "//cc/bzd/core/assert",
        "//cc/bzd/test",
        "//cc/bzd/utility/pattern:to_stream",
        "//cc/bzd/utility/pattern:to_string",
    ],
)
