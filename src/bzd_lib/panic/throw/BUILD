config_setting(
    name = "backend",
    define_values = {
        "panic": "throw",
    },
    visibility = ["//visibility:public"],
)

cc_library(
    name = "throw",
    srcs = glob([
        "*.cpp",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//src:bzd_core",
    ],
)
