package(default_visibility = ["//visibility:public"])

filegroup(
    name = "linker_files",
    srcs = [
        ":static_libraries_files",
    ] + glob([
        "bin/*",
        "ld/*",
    ])
)

filegroup(
    name = "static_libraries_files",
    srcs = glob([
        "lib/*",
    ]),
)

filegroup(
    name = "compiler_files",
    srcs = glob([
        "include/**/*",
    ])
)
