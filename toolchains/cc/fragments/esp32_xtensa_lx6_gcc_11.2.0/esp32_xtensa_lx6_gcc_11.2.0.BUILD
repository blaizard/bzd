package(default_visibility = ["//visibility:public"])

filegroup(
    name = "all",
    srcs = [
        ":includes",
        ":bin",
        ":static_libraries",
    ],
)

filegroup(
    name = "includes",
    srcs = glob([
        "include/**",
        "lib/gcc/xtensa-esp32-elf/11.2.0/include/**",
        "xtensa-esp32-elf/include/**",
        "**/*",
    ]),
)

filegroup(
    name = "bin",
    srcs = glob([
        "bin/*",
    ]),
)

filegroup(
    name = "static_libraries",
    srcs = glob([
        "xtensa-esp32-elf/lib/**",
    ]),
)
