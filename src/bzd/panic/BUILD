cc_library(
    name = "panic",
    visibility = ["//visibility:public"],
    deps = select({
        "//src/bzd/panic/throw:backend": ["//src/bzd/panic/throw"],
        "//src/bzd/panic/trap:backend": ["//src/bzd/panic/trap"],
        "//src/bzd/panic/exit:backend": ["//src/bzd/panic/exit"],
        "//conditions:default": ["//src/bzd/panic/exit"],
    }),
)
