package(default_visibility = ["//visibility:public"])

filegroup(
    name = "includes",
    srcs = glob([
        "usr/lib/gcc/x86_64-linux-gnu/11/include/**/*",
        "usr/lib/gcc/x86_64-linux-gnu/11/include-fixed/**/*",
        "usr/include/x86_64-linux-gnu/c++/11/**/*",
        "usr/include/c++/11/**/*",
    ]),
)

filegroup(
    name = "bin",
    srcs = glob([
        "usr/bin/*",
        "usr/lib/gcc/x86_64-linux-gnu/11/**/*",
        "usr/lib/x86_64-linux-gnu/**/*"
    ]),
)

filegroup(
    name = "dynamic_libraries",
    srcs = glob([
        "usr/lib/gcc/x86_64-linux-gnu/11/libstdc++.so",
        "usr/lib/x86_64-linux-gnu/**/*"
    ]),
)

filegroup(
    name = "static_libraries",
    srcs = glob([
        "usr/lib/**/*.a",
    ]),
)
