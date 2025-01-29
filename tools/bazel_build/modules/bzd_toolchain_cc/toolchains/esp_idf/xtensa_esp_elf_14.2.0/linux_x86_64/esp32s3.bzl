"""Metadata for ESP32S3 toolchains."""

load("//cc:toolchain.bzl", "toolchain_maker", "toolchain_merge")
load("//toolchains/esp_idf:sdk/esp32s3.bzl", "sdk")
load("//toolchains/esp_idf/app_binary:esp32s3.bzl", "app_binary")
load("//toolchains/esp_idf/esptool:defs.bzl", "esptool")
load("//toolchains/esp_idf/qemu:defs.bzl", "esp32_qemu")

def esp32s3_linux_x86_64(name, use_fragment):
    """Metadata for ESP32S3 toolchains.

    Args:
        name: The name of the toolchain.
        use_fragment: Factory to make use of a fragment.
    """

    use_fragment(esptool)
    use_fragment(esp32_qemu)
    _sdk_name, sdk_toolchain_definition = use_fragment(sdk)
    _app_binary_name, app_binary_toolchain_definition = use_fragment(app_binary)

    toolchain_definition = {
        "binaries": {
            "ar": "xtensa-esp-elf/bin/ar",
            "as": "xtensa-esp-elf/bin/as",
            "cc": "bin/wrapper_cc_start_end_group",
            "cov": "bin/xtensa-esp32s3-elf-gcov",
            "cpp": "bin/xtensa-esp32s3-elf-gcc",
            "ld": "xtensa-esp-elf/bin/ld",
            "objcopy": "xtensa-esp-elf/bin/objcopy",
            "objdump": "xtensa-esp-elf/bin/objdump",
            "strip": "xtensa-esp-elf/bin/strip",
        },
        "build_files": [
            Label("//toolchains/esp_idf:xtensa_esp_elf_14.2.0/linux_x86_64/esp32s3.BUILD"),
        ],
        "compile_flags": [
            # Allow long calls
            "-mlongcalls",
            # Standard includes, this is needed to avoid indefined include complain from Bazel.
            "-nostdinc",
            "--no-standard-includes",
            # Needed for clang-tdy
            "-D__XTENSA__",
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
        "package_name": "xtensa-esp32-elf",
        "patches": [
            Label("//toolchains/esp_idf:xtensa_esp_elf_14.2.0/linux_x86_64/esp32s3_wrapper_cc_start_end_group.patch"),
        ],
        "sha256": "e3e6dcf3d275c3c9ab0e4c8a9d93fd10e7efc035d435460576c9d95b4140c676",
        "strip_prefix": "xtensa-esp-elf",
        "system_directories": [
            "@{}//xtensa-esp-elf/include/c++/14.2.0".format(name),
            "@{}//xtensa-esp-elf/include/c++/14.2.0/xtensa-esp-elf/esp32s3".format(name),
            "@{}//xtensa-esp-elf/include/c++/14.2.0/backward".format(name),
            "@{}//lib/gcc/xtensa-esp-elf/14.2.0/include".format(name),
            "@{}//lib/gcc/xtensa-esp-elf/14.2.0/include-fixed".format(name),
            "@{}//xtensa-esp-elf/include".format(name),
        ],
        "urls": [
            "https://datalocal.blaizard.com/file/bzd/toolchains/cc/gcc/esp32_xtensa_lx6/xtensa-esp-elf-14.2.0_20241119-x86_64-linux-gnu.tar.xz",
            "https://data.blaizard.com/file/bzd/toolchains/cc/gcc/esp32_xtensa_lx6/xtensa-esp-elf-14.2.0_20241119-x86_64-linux-gnu.tar.xz",
        ],
    }

    # Note, the order is important here. We want the definition of the SDK to have precedence over the
    # toolchain: includes from the SDK should have higher priority than the ones from the toolchain.
    toolchain_definition = toolchain_merge(sdk_toolchain_definition, toolchain_definition)
    toolchain_definition = toolchain_merge(app_binary_toolchain_definition, toolchain_definition)

    toolchain_maker(
        name = name,
        implementation = "linux_gcc",
        definition = toolchain_definition,
    )
