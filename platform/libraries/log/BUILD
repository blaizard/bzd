cc_library(
    name = "log",
    visibility = ["//visibility:public"],
    deps = select({
        "//platform/libraries/log/stdout:backend": ["//platform/libraries/log/stdout"],
        "//conditions:default": ["//platform/libraries/log/stdout"],
    }),
)
