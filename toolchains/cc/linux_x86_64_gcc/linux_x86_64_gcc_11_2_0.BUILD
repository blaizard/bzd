package(default_visibility = ["//visibility:public"])

filegroup(
    name = "all",
    srcs = [
        ":includes",
        ":bin",
        ":dynamic_libraries",
        ":static_libraries",        
    ],
)

filegroup(
    name = "includes",
    srcs = glob([
        "lib/gcc/x86_64-pc-linux-gnu/11.2.0/include/**/*",
        "lib/gcc/x86_64-pc-linux-gnu/11.2.0/include-fixed/**/*",
        "include/c++/11.2.0/**/*",
    ]),
)

filegroup(
    name = "bin",
    srcs = glob([
        "bin/*",
        "libexec/gcc/x86_64-pc-linux-gnu/11.2.0/**/*",
    ]),
)

filegroup(
    name = "dynamic_libraries",
    srcs = glob([
        "lib64/**/*",
        "lib/**/*",

    ]),
)

filegroup(
    name = "static_libraries",
    srcs = glob([
        "lib64/**/*",
        "lib/**/*",
    ]),
)
