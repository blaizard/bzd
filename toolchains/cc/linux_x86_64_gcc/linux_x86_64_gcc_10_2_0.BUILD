package(default_visibility = ["//visibility:public"])

filegroup(
    name = "includes",
    srcs = glob([
        "usr/lib/gcc/x86_64-linux-gnu/10/include/**/*",
        "usr/lib/gcc/x86_64-linux-gnu/10/include-fixed/**/*",
        "usr/include/x86_64-linux-gnu/c++/10/**/*",
        "usr/include/c++/10/**/*",
    ]),
)

filegroup(
    name = "bin",
    srcs = glob([
        "usr/bin/*",
        "usr/lib/gcc/x86_64-linux-gnu/10/**/*",
        "usr/lib/x86_64-linux-gnu/**/*"
    ]),
)

filegroup(
    name = "dynamic_libraries",
    srcs = glob([
        "usr/lib/gcc/x86_64-linux-gnu/10/libstdc++.so",
        "usr/lib/x86_64-linux-gnu/**/*"
    ]),
)

filegroup(
    name = "static_libraries",
    srcs = glob([
        "usr/lib/**/*.a",
    ]),
)
