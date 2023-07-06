load("@//toolchains/cc/linux_x86_64_gcc:defs.bzl", "BINARIES")

package(default_visibility = ["//visibility:public"])

[alias(
    name = key,
    actual = path
) for key, path in BINARIES.items()]

filegroup(
    name = "ar_files",
    srcs = [
        ":ar",
    ]
)

filegroup(
    name = "as_files",
    srcs = [
        ":as",
    ]
)

filegroup(
    name = "compiler_files",
    srcs = [
        ":cc",
        ":cpp",
        ":cov",
        ":header_files",
    ] + glob(["libexec/gcc/x86_64-pc-linux-gnu/11.2.0/*",])
)

filegroup(
    name = "linker_files",
    srcs = [
        ":cc",
        ":ld",
        ":dynamic_libraries_files",
        ":static_libraries_files",
    ] + glob([
        "libexec/gcc/x86_64-pc-linux-gnu/11.2.0/*",
        "lib/gcc/x86_64-pc-linux-gnu/11.2.0/*"
    ])
)

filegroup(
    name = "objcopy_files",
    srcs = [
        ":objcopy",
    ]
)

filegroup(
    name = "strip_files",
    srcs = [
        ":strip",
    ]
)

filegroup(
    name = "dynamic_libraries_files",
    srcs = glob([
        "lib64/*.so*",
    ]),
)

filegroup(
    name = "static_libraries_files",
    srcs = glob([
        "lib64/*.a",
    ]),
)

filegroup(
    name = "header_files",
    srcs = glob([
        "lib/gcc/x86_64-pc-linux-gnu/11.2.0/include/**/*",
        "lib/gcc/x86_64-pc-linux-gnu/11.2.0/include-fixed/**/*",
        "include/c++/11.2.0/**/*",
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
