config_setting(
    name = "backend",
    define_values = {
        "log": "stdout",
    },
    visibility = ["//visibility:public"],
)

cc_library(
    name = "stdout",
    srcs = glob([
        "*.cpp",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//include/bzd:include",
    ],
)
