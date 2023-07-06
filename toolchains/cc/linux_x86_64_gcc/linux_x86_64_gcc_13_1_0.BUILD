load("@//toolchains/cc/linux_x86_64_gcc:defs.bzl", "BINARIES")

package(default_visibility = ["//visibility:public"])

[alias(
    name = key,
    actual = path
) for key, path in BINARIES.items()]

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
    ] + glob(["libexec/gcc/x86_64-pc-linux-gnu/13.1.0/*",])
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
        "libexec/gcc/x86_64-pc-linux-gnu/13.1.0/*",
        "lib/gcc/x86_64-pc-linux-gnu/13.1.0/*"
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
        "lib/gcc/x86_64-pc-linux-gnu/13.1.0/include/**/*",
        "lib/gcc/x86_64-pc-linux-gnu/13.1.0/include-fixed/**/*",
        "include/c++/13.1.0/**/*",
    ])
)

# Create a package with only what is needed
genrule(
    name = "package",
    srcs = ["@linux_x86_64_gcc//:all_files"],
    outs = ["linux_x86_64_13.1.0.tar.xz"],
    tags = ["manual"],
    cmd = """
    tar -cJf $@ --dereference --transform 's/^\\./linux_x86_64_13.1.0/' -C external/linux_x86_64_gcc_13_1_0 .
    """
)
