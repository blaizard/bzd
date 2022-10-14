load("//toolchains/cc/linux_x86_64_clang:defs.bzl", "load_linux_x86_64_clang")
load("//toolchains/cc/linux_x86_64_gcc:defs.bzl", "load_linux_x86_64_gcc")
load("//toolchains/cc/esp32_xtensa_lx6_gcc:defs.bzl", "load_esp32_xtensa_lx6_gcc")
load("//toolchains/python/linux_x86_64:defs.bzl", "load_linux_x86_64_python")
load("//toolchains/nodejs/linux_x86_64_yarn:defs.bzl", "load_linux_x86_64_yarn")

def toolchains_register():
    load_linux_x86_64_clang()
    load_linux_x86_64_gcc()
    load_esp32_xtensa_lx6_gcc()
    load_linux_x86_64_python()
    load_linux_x86_64_yarn()

    # Register default binary toolchain
    native.register_toolchains(
        "//toolchains:binary_toolchain",
    )
