filegroup(
    name = "base_files",
    srcs = glob([
        "lib/*.so*",
    ]),
)

filegroup(
    name = "ar_files",
    srcs = [
        ":ar",
        ":base_files"
    ],
)

filegroup(
    name = "as_files",
    srcs = [
        ":as",
        ":base_files"
    ]
)

filegroup(
    name = "compiler_files",
    srcs = [
        ":cc",
        ":cpp",
        ":cov",
        ":header_files",
        ":base_files"
    ] + glob(["libexec/gcc/x86_64-pc-linux-gnu/14.2.0/*",])
)

filegroup(
    name = "linker_files",
    srcs = [
        ":cc",
        ":ld",
        ":dynamic_libraries_files",
        ":static_libraries_files",
        ":base_files"
    ] + glob([
        "libexec/gcc/x86_64-pc-linux-gnu/14.2.0/*",
        "lib/gcc/x86_64-pc-linux-gnu/14.2.0/*"
    ])
)

filegroup(
    name = "objcopy_files",
    srcs = [
        ":objcopy",
        ":base_files"
    ]
)

filegroup(
    name = "strip_files",
    srcs = [
        ":strip",
        ":base_files"
    ]
)

filegroup(
    name = "dynamic_libraries_files",
    srcs = [
        ":base_files"
    ] + glob([
        "lib64/*.so*",
    ]),
)

filegroup(
    name = "static_libraries_files",
    srcs = [
        ":base_files"
    ] + glob([
        "lib64/*.a",
    ]),
)

filegroup(
    name = "header_files",
    srcs = glob([
        "lib/gcc/x86_64-pc-linux-gnu/14.2.0/include/**/*",
        "lib/gcc/x86_64-pc-linux-gnu/14.2.0/include-fixed/**/*",
        "include/c++/14.2.0/**/*",
    ])
)
