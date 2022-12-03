load("@//toolchains/cc/linux_x86_64_clang:defs.bzl", "BINARIES")

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
        ":header_files",
    ]
)

filegroup(
    name = "linker_files",
    srcs = [
        ":cc",
        ":ld",
        "bin/lld",
        "bin/ld.lld",
        ":dynamic_libraries_files",
        ":static_libraries_files",
    ]
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
        "lib/*.so*",
    ]),
)

filegroup(
    name = "static_libraries_files",
    srcs = glob([
        "lib/**/*.a",
    ]),
)

filegroup(
    name = "header_files",
    srcs = glob([
        "include/c++/v1/**/*",
        "lib/clang/15.0.6/include/**/*",
        "include/x86_64-unknown-linux-gnu/c++/v1/**/*"
    ])
)

filegroup(
    name = "clang_format",
    srcs = ["bin/clang-format"],
)

filegroup(
    name = "llvm_symbolizer",
    srcs = ["bin/llvm-symbolizer"],
)

sh_binary(
    name = "clang_tidy",
    srcs = [
        "bin/clang-tidy"
    ],
    data = [
        ":header_files"
    ]
)

# Create a package with only what is needed
genrule(
    name = "package",
    srcs = [
        "@linux_x86_64_clang//:all_files",
        ":clang_format",
        ":clang_tidy",
        ":llvm_symbolizer",
    ],
    outs = ["linux_x86_64_15.0.6.tar.xz"],
    tags = ["manual"],
    cmd = """
    tar -cJf $@ --dereference --transform 's/^\\./linux_x86_64_15.0.6/' -C external/linux_x86_64_clang_15_0_6 .
    """
)
