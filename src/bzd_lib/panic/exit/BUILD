config_setting(
    name = "backend",
    define_values = {
        "panic": "exit",
    },
    visibility = ["//visibility:public"],
)

cc_library(
    name = "exit",
    srcs = glob([
        "*.cpp",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//src:bzd_core",
    ],
)
