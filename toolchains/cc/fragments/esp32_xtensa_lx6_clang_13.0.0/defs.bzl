load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def toolchain_fragment_esp32_xtensa_lx6_clang():
    package_name = "esp32_xtensa_lx6_clang_13.0.0"

    http_archive(
        name = package_name,
        build_file = "//toolchains/cc/fragments/esp32_xtensa_lx6_clang_13.0.0:{}.BUILD".format(package_name),
        urls = [
            "https://github.com/espressif/llvm-project/releases/download/esp-13.0.0-20211203/xtensa-esp32-elf-llvm13_0_0-esp-13.0.0-20211203-linux-amd64.tar.xz",
        ],
        strip_prefix = "xtensa-esp32-elf-clang",
        sha256 = "312a8b3e0765066b10e8f2654d09fb0c3cd5f96d5328b94ad703cf71bd8ebf9d",
    )

    return {
        "cpu": "linux_x86_64",
        "compiler": "clang",
        "builtin_include_directories": [
            "lib/clang/13.0.0/include",
            "xtensa-esp32-elf/include",
            "xtensa-esp32-elf/include/c++/8.4.0",
            "xtensa-esp32-elf/include/c++/8.4.0/xtensa-esp32-elf",
            "xtensa-esp32-elf/sys-include",
        ],
        "system_directories": [
            "external/{}/lib/clang/13.0.0/include".format(package_name),
            "external/{}/xtensa-esp32-elf/include".format(package_name),
            "external/{}/xtensa-esp32-elf/include/c++/8.4.0".format(package_name),
            "external/{}/xtensa-esp32-elf/include/c++/8.4.0/xtensa-esp32-elf".format(package_name),
            "external/{}/xtensa-esp32-elf/sys-include".format(package_name),
        ],
        "linker_dirs": [
            "external/{}/xtensa-esp32-elf/lib".format(package_name),
            "external/{}/lib".format(package_name),
        ],
        "compile_flags": [
            "--target=xtensa",
            "-mcpu=esp32",
            # -freestanding is a hack to force Clang to use its own stdatomic.h,
            # without falling back to the (incompatible) GCC stdatomic.h
            "-ffreestanding",

            # Use C++17
            "-std=c++17",

            # Standard includes, this is needed to avoid indefined include complain from Bazel.
            "-nostdinc",
            "--no-standard-includes",

            # Add support to coroutines.
            "-Wno-unused-command-line-argument",
            "-stdlib=libc++",
            "-fcoroutines-ts",
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
        "dynamic_runtime_libs": [
            "@{}//:dynamic_libraries".format(package_name),
        ],
        "static_runtime_libs": [
            "@{}//:static_libraries".format(package_name),
        ],
        "filegroup_dependencies": [
            "@{}//:includes".format(package_name),
            "@{}//:bin".format(package_name),
        ],
        "bin_ar": "external/{}/bin/xtensa-esp32-elf-ar".format(package_name),
        "bin_as": "external/{}/bin/xtensa-esp32-elf-as".format(package_name),
        "bin_cc": "external/{}/bin/clang".format(package_name),
        "bin_cpp": "external/{}/bin/clang".format(package_name),
        "bin_cov": "external/{}/bin/xtensa-esp32-elf-cov".format(package_name),
        "bin_objdump": "external/{}/bin/xtensa-esp32-elf-objdump".format(package_name),
        "bin_ld": "external/{}/bin/clang++".format(package_name),
        "bin_strip": "external/{}/bin/xtensa-esp32-elf-strip".format(package_name),
    }
