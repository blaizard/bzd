package(default_visibility = ["//visibility:public"])

filegroup(
    name = "includes",
    srcs = glob([
        "include/c++/v1/**/*",
        "lib/clang/9.0.0/include/**/*"
    ]),
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
        "lib/*.so*"
    ]),
)

filegroup(
    name = "static_libraries",
    srcs = glob([
        "lib/*.a"
    ]),
)
