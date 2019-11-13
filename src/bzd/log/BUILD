cc_library(
    name = "log",
    visibility = ["//visibility:public"],
    deps = select({
        "//src/bzd/log/stdout:backend": ["//src/bzd/log/stdout"],
        "//conditions:default": ["//src/bzd/log/stdout"],
    }),
)
