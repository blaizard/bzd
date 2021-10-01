load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "backend",
    srcs = [
        "backend.cc",
    ],
    tags = ["manual"],
    visibility = [
        "//cc/bzd/platform/backend:__pkg__",
    ],
    deps = [
        "//cc/bzd/platform/esp32_xtensa_lx6/clock",
        "//cc/bzd/platform/generic/panic/trap",
        "//cc/bzd/platform/generic/stream/stub",
        "//cc/bzd/platform/std/stream/out",
    ],
)
