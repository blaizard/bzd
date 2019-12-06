filegroup(
    name = "bzd_files",
    srcs = glob([
        "bzd/**/*.h",
    ]),
    visibility = ["//visibility:public"],
)

cc_library(
    name = "bzd",
    hdrs = [
        ":bzd_files",
    ],
    includes = ["."],
    visibility = ["//visibility:public"],
)
