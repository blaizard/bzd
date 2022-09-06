load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "http",
    hdrs = [
        "http.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc:bzd",
        "//cc/bzd/utility/synchronization:lock_guard",
        "//cc/bzd/utility/synchronization:mutex",
    ],
)
