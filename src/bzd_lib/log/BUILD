cc_library(
    name = "log",
    visibility = ["//visibility:public"],
    deps = select({
        "//src/bzd_lib/log/stdout:backend": ["//src/bzd_lib/log/stdout"],
        "//conditions:default": ["//src/bzd_lib/log/stdout"],
    }),
)
