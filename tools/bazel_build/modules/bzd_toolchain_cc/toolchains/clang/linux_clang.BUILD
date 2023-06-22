package(default_visibility = ["//visibility:public"])

alias(
    name = "ar",
    actual = "bin/llvm-ar",
)

alias(
    name = "as",
    actual = "bin/llvm-as",
)

alias(
    name = "cc",
    actual = "bin/clang",
)

alias(
    name = "cpp",
    actual = "bin/clang",
)

alias(
    name = "cov",
    actual = "bin/llvm-cov",
)

alias(
    name = "objcopy",
    actual = "bin/llvm-objcopy",
)

alias(
    name = "objdump",
    actual = "bin/llvm-objdump",
)

alias(
    name = "ld",
    actual = "bin/clang++",
)

alias(
    name = "strip",
    actual = "bin/llvm-strip",
)

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
