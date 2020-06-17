package(default_visibility = ["//visibility:public"])

filegroup(
    name = "includes",
    srcs = glob([
        "usr/lib/gcc/x86_64-linux-gnu/8/include/**/*",
        "usr/lib/gcc/x86_64-linux-gnu/8/include-fixed/**/*",
        "usr/include/x86_64-linux-gnu/c++/8/**/*",
        "usr/include/c++/8/**/*",
    ]),
)

filegroup(
    name = "bin",
    srcs = glob([
        "usr/bin/*",
        "usr/lib/gcc/x86_64-linux-gnu/8/**/*",
        "usr/lib/x86_64-linux-gnu/**/*"
    ]),
)

filegroup(
    name = "dynamic_libraries",
    srcs = glob([
        "usr/lib/gcc/x86_64-linux-gnu/8/libstdc++.so",
        "usr/lib/x86_64-linux-gnu/**/*"
    ]),
)

filegroup(
    name = "static_libraries",
    srcs = glob([
        "usr/lib/**/*.a",
    ]),
)
