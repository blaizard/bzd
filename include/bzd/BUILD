cc_library(
    name = "include",
    hdrs = glob([
        "**/*.h",
    ]),
    visibility = ["//visibility:public"],
    includes = [".."]
)

cc_library(
    name = "log_backend",
    visibility = ["//visibility:public"],
    deps = select({
        "//src/log/stdout:backend": ["//src/log/stdout"],
        "//conditions:default": ["//src/log/stdout"],
    }),
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
    name = "bzd",
    visibility = ["//visibility:public"],
    deps = [
        ":include",
        ":log_backend",
        ":panic_backend",
    ],
)
