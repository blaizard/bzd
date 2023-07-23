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
    name = "extra_files",
    srcs = [
        ":clang_format",
        ":llvm_symbolizer",
        ":clang_tidy"
    ],
)

filegroup(
    name = "clang_format",
    srcs = ["bin/clang-format"],
)

filegroup(
    name = "llvm_symbolizer",
    srcs = ["bin/llvm-symbolizer"],
)

sh_binary_wrapper(
    name = "clang_tidy",
    locations = {
        "bin/clang-tidy": "clang_tidy",
        ":llvm_symbolizer": "llvm_symbolizer"
    },
    command = """
export LLVM_SYMBOLIZER_PATH="{llvm_symbolizer}"
{clang_tidy} $@
    """,
    data = [
        ":header_files"
    ],
    visibility = ["//visibility:public"],
)

sh_binary_wrapper(
    name = "executor_host",
    locations = {
        ":llvm_symbolizer": "llvm_symbolizer"
    },
    command = """
export LLVM_SYMBOLIZER_PATH="{llvm_symbolizer}"
$@
    """,
    visibility = ["//visibility:public"],
)
