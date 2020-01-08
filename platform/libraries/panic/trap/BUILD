config_setting(
    name = "backend",
    define_values = {
        "panic": "trap",
    },
    visibility = ["//visibility:public"],
)

cc_library(
    name = "trap",
    srcs = glob([
        "*.cpp",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//platform:bzd_core",
    ],
)
