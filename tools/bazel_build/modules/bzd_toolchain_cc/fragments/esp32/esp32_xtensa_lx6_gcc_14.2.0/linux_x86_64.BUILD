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
        "bin/xtensa-esp32-elf-gcc",
        "@esp32_xtensa_lx6_sdk//:compiler_files",
    ] + glob([
        "libexec/gcc/xtensa-esp-elf/14.2.0/*",
        "xtensa-esp-elf/bin/*",
    ])
)

filegroup(
    name = "linker_files",
    srcs = [
        ":cc",
        ":ld",
        ":dynamic_libraries_files",
        ":static_libraries_files",
        "@esp32_xtensa_lx6_sdk//:linker_files",
        "bin/xtensa-esp32-elf-gcc",
    ] + glob([
        "libexec/gcc/xtensa-esp-elf/14.2.0/*",
        "lib/gcc/xtensa-esp-elf/14.2.0/*"
    ])
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
    srcs = [],
)

filegroup(
    name = "static_libraries_files",
    srcs = [
        "@esp32_xtensa_lx6_sdk//:static_libraries_files"
    ] + glob([
        "xtensa-esp-elf/lib/**",
    ]),
)

filegroup(
    name = "header_files",
    srcs = glob([
        "lib/gcc/xtensa-esp-elf/14.2.0/include/**",
        "lib/gcc/xtensa-esp-elf/14.2.0/include-fixed/**",
        "xtensa-esp-elf/include/c++/14.2.0/xtensa-esp-elf/**",
        "xtensa-esp-elf/include/c++/14.2.0/**",
        "xtensa-esp-elf/include/**",
    ])
)
