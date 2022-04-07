package(default_visibility = ["//visibility:public"])

filegroup(
    name = "includes",
    srcs = glob([
        "include/c++/v1/**/*",
        "lib/clang/14.0.0/include/**/*",
        "include/x86_64-unknown-linux-gnu/c++/v1/**/*"
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
        ":includes"
    ]
)

filegroup(
    name = "bin",
    srcs = glob([
        "bin/*",
    ]),
)

filegroup(
    name = "dynamic_libraries",
    srcs = glob([
        "lib/*.so*",
    ]),
)

filegroup(
    name = "static_libraries",
    srcs = glob([
        "lib/**/*.a",
    ]),
)