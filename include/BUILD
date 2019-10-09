cc_library(
    name = "include",
    hdrs = glob([
        "**/*.h",
    ]),
    visibility = ["//visibility:public"],
)

cc_library(
    name = "panic_backend",
    visibility = ["//visibility:public"],
    deps = select({
        "//src/panic/throw:backend": ["//src/panic/throw"],
        "//src/panic/trap:backend": ["//src/panic/trap"],
        "//src/panic/exit:backend": ["//src/panic/exit"],
        "//conditions:default": ["//src/panic/exit"],
    }),
)

cc_library(
    name = "lib",
    visibility = ["//visibility:public"],
    deps = [
        ":include",
        ":panic_backend",
    ],
)

cc_test(
    name = "tests",
    srcs = glob([
        "**/*.cpp",
    ]),
    deps = [
        ":lib",
        "@googletest//:gtest_main",
    ],
)
