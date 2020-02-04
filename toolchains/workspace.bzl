load("//toolchains/linux_x86_64_clang:defs.bzl", "load_linux_x86_64_clang")
load("//toolchains/esp32_xtensa_lx6_gcc:defs.bzl", "load_esp32_xtensa_lx6_gcc")

def load_toolchains():
    load_linux_x86_64_clang()
    load_esp32_xtensa_lx6_gcc()

    # Register default app toolchain
    native.register_toolchains(
        "//toolchains:app_toolchain",
    )
