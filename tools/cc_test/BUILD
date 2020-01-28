cc_library(
    name = "cc_test",
    hdrs = [
        "test_internal.h",
        "test.h",
    ],
    srcs = [
        "test.cpp",
    ],
    alwayslink = True,
    includes = ["."],
	include_prefix = "cc_test",
    visibility = ["//visibility:public"],
    deps = [
        "//platform:sdk"
    ]
)
