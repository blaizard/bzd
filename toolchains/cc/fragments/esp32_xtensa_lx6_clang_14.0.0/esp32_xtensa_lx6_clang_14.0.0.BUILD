package(default_visibility = ["//visibility:public"])

filegroup(
    name = "includes",
    srcs = glob([
        "xtensa-esp32-elf/include/**/*",
        "xtensa-esp32-elf/sys-include/**/*",
        "lib/clang/14.0.0/include/**/*",
        "include/**/*"
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
        "lib/**/*.so*",
        "xtensa-esp32-elf/lib/**/*.o"
    ]),
)

filegroup(
    name = "static_libraries",
    srcs = glob([
        "lib/**/*.a",
        "xtensa-esp32-elf/lib/**/*.a"
    ]),
)
