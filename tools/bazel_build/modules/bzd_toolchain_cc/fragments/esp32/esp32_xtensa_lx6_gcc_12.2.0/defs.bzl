"""Metadata for ESP32 toolchains."""

load("//:fragments/esp32/esp32_xtensa_lx6_sdk/defs.bzl", "esp32_xtensa_lx6_sdk")
load("//cc:toolchain.bzl", "get_location", "toolchain_maker", "toolchain_merge")
load("//fragments/esp32/app_binary/esp32_xtensa_lx6:defs.bzl", "app_binary")

def linux_x86_64(module_ctx, name):
    """Metadata for ESP32 toolchains.

    Args:
        module_ctx: The module context.
        name: The name of the toolchain.
    """

    repository_path = get_location(module_ctx, name)
    toolchain_definition = {
        "binaries": {
            "ar": "xtensa-esp32-elf/bin/ar",
            "as": "xtensa-esp32-elf/bin/as",
            "cc": "bin/wrapper_cc_start_end_group",
            "cov": "bin/xtensa-esp32-elf-gcov",
            "cpp": "bin/xtensa-esp32-elf-gcc",
            "ld": "xtensa-esp32-elf/bin/ld",
            "objcopy": "xtensa-esp32-elf/bin/objcopy",
            "objdump": "xtensa-esp32-elf/bin/objdump",
            "strip": "xtensa-esp32-elf/bin/strip",
        },
        "build_files": [
            "@bzd_toolchain_cc//:fragments/esp32/esp32_xtensa_lx6_gcc_12.2.0/linux_x86_64.BUILD",
        ],
        "builtin_include_directories": [
            "%sysroot%/xtensa-esp32-elf/include/c++/12.2.0/xtensa-esp32-elf",
            "%sysroot%/xtensa-esp32-elf/include/c++/12.2.0",
            "%sysroot%/xtensa-esp32-elf/sys-include",
            "%sysroot%/lib/gcc/xtensa-esp32-elf/12.2.0/include",
            "%sysroot%/lib/gcc/xtensa-esp32-elf/12.2.0/include-fixed",
        ],
        "compile_flags": [
            # Allow long calls
            "-mlongcalls",
        ],
        "cxx_flags": [
            "-std=c++20",
        ],
        "link_flags": [
            # Do not link with the stdlib
            "-nostdlib",

            # Link little-endian objects
            "-Wl,-EL",

            # Do not link with shared libraries
            "-Wl,-static",
        ],
        "linker_dirs": [
            "{}/xtensa-esp32-elf/lib".format(repository_path),
        ],
        "package_name": "xtensa-esp32-elf",
        "patches": [
            "@bzd_toolchain_cc//:fragments/esp32/esp32_xtensa_lx6_gcc_12.2.0/wrapper_cc_start_end_group.patch",
        ],
        "sha256": "4d2e02ef47f1a93a4dcfdbaecd486adfaab4c0e26deea2c18d6385527f39f864",
        "strip_prefix": "xtensa-esp32-elf",
        "sysroot": True,
        "urls": [
            "https://datalocal.blaizard.com/file/bzd/toolchains/cc/gcc/esp32_xtensa_lx6/xtensa-esp32-elf-12.2.0_20230208-x86_64-linux-gnu.tar.xz",
            "https://data.blaizard.com/file/bzd/toolchains/cc/gcc/esp32_xtensa_lx6/xtensa-esp32-elf-12.2.0_20230208-x86_64-linux-gnu.tar.xz",
        ],
    }

    toolchain_definition = toolchain_merge(toolchain_definition, esp32_xtensa_lx6_sdk(module_ctx))
    toolchain_definition = toolchain_merge(toolchain_definition, app_binary(module_ctx))

    toolchain_maker(
        name = name,
        implementation = "linux_gcc",
        definition = toolchain_definition,
    )

esp32_xtensa_lx6_gcc_12_2_0 = {
    "linux-x86_64": linux_x86_64,
}
