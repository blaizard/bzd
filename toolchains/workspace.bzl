load("//toolchains/cc/linux_x86_64_clang:defs.bzl", "load_linux_x86_64_clang")
load("//toolchains/cc/esp32_xtensa_lx6_gcc:defs.bzl", "load_esp32_xtensa_lx6_gcc")
load("//toolchains/python/linux_x86_64:defs.bzl", "load_linux_x86_64_python")

def load_toolchains():
    load_linux_x86_64_clang()
    load_esp32_xtensa_lx6_gcc()
    load_linux_x86_64_python()

    # Register default app toolchain
    native.register_toolchains(
        "//toolchains:app_toolchain",
    )
