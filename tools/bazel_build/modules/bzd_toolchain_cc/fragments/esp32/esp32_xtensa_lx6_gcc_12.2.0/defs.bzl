load("@bzd_toolchain_cc//cc:toolchain.bzl", "get_location", "toolchain_maker", "toolchain_merge")
load("@bzd_toolchain_cc//:fragments/esp32/esp32_xtensa_lx6_sdk/defs.bzl", "esp32_xtensa_lx6_sdk")
load("@bzd_toolchain_cc//fragments/esp32/esptool:defs.bzl", "esptool")

def linux_x86_64(module_ctx, name):
    repository_path = get_location(module_ctx, name)
    toolchain_definition = {
        "url": "http://data.blaizard.com/file/bzd/toolchains/cc/gcc/esp32_xtensa_lx6/xtensa-esp32-elf-12.2.0_20230208-x86_64-linux-gnu.tar.xz",
        "strip_prefix": "xtensa-esp32-elf",
        "sha256": "4d2e02ef47f1a93a4dcfdbaecd486adfaab4c0e26deea2c18d6385527f39f864",
        "build_files": [
            "@bzd_toolchain_cc//:fragments/esp32/esp32_xtensa_lx6_gcc_12.2.0/linux_x86_64.BUILD",
        ],
        "patches": [
            "@bzd_toolchain_cc//:fragments/esp32/esp32_xtensa_lx6_gcc_12.2.0/wrapper_cc_start_end_group.patch",
        ],
        "system_directories": [
            "{}/xtensa-esp32-elf/include/c++/12.2.0/xtensa-esp32-elf".format(repository_path),
            "{}/xtensa-esp32-elf/include/c++/12.2.0".format(repository_path),
            "{}/xtensa-esp32-elf/sys-include".format(repository_path),
            "{}/lib/gcc/xtensa-esp32-elf/12.2.0/include".format(repository_path),
            "{}/lib/gcc/xtensa-esp32-elf/12.2.0/include-fixed".format(repository_path),
        ],
        "linker_dirs": [
            "{}/xtensa-esp32-elf/lib".format(repository_path),
        ],
        "compile_flags": [
            # Use C++20.
            "-std=c++20",

            # Do not link or re-order inclusion files
            "-nostdinc",
            "-nostdinc++",
            "--no-standard-includes",

            # Allow long calls
            "-mlongcalls",
        ],
        "link_flags": [
            # No standard libraries
            "-nostdlib",

            # Link little-endian objects
            "-Wl,-EL",

            # Garbage collection
            "-Wl,--gc-sections",

            # Do not link with shared libraries
            "-Wl,-static",
        ],
        "binaries": {
            "ar": "xtensa-esp32-elf/bin/ar",
            "as": "xtensa-esp32-elf/bin/as",
            "cc": "bin/wrapper_cc_start_end_group",
            "cpp": "bin/xtensa-esp32-elf-gcc",
            "cov": "bin/xtensa-esp32-elf-gcov",
            "objcopy": "xtensa-esp32-elf/bin/objcopy",
            "objdump": "xtensa-esp32-elf/bin/objdump",
            "ld": "xtensa-esp32-elf/bin/ld",
            "strip": "xtensa-esp32-elf/bin/strip",
        },
    }

    toolchain_definition = toolchain_merge(toolchain_definition, esptool(module_ctx))
    toolchain_definition = toolchain_merge(toolchain_definition, esp32_xtensa_lx6_sdk(module_ctx))

    toolchain_maker(
        name = name,
        implementation = "linux_gcc",
        definition = toolchain_definition,
    )

esp32_xtensa_lx6_gcc_12_2_0 = {
    "linux-x86_64": linux_x86_64,
}
