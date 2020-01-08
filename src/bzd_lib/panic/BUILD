cc_library(
    name = "panic",
    visibility = ["//visibility:public"],
    deps = select({
        "//src/bzd_lib/panic/throw:backend": ["//src/bzd_lib/panic/throw"],
        "//src/bzd_lib/panic/trap:backend": ["//src/bzd_lib/panic/trap"],
        "//src/bzd_lib/panic/exit:backend": ["//src/bzd_lib/panic/exit"],
        "//conditions:default": ["//src/bzd_lib/panic/exit"],
    }),
)
