cc_library(
    name = "include",
    hdrs = glob([
        "**/*.h",
    ]),
    copts = ["-std=c++14"],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "tests",
    srcs = glob([
        "**/*.cpp",
    ]),
    copts = ["-std=c++14"],
    deps = [
        ":include",
        "@googletest//:gtest_main",
    ],
)
