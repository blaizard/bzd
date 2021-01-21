load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("//toolchains/cc:defs.bzl", "COPTS_GCC", "LINKOPTS_GCC")

def toolchain_fragment_esp32_xtensa_lx6_gcc():
    package_name = "esp32_xtensa_lx6_gcc_8.2.0"

    http_archive(
        name = package_name,
        build_file = "//toolchains/cc/fragments/esp32_xtensa_lx6_gcc:{}.BUILD".format(package_name),
        urls = [
            "https://dl.espressif.com/dl/xtensa-esp32-elf-gcc8_2_0-esp-2019r2-linux-amd64.tar.gz",
        ],
        strip_prefix = "xtensa-esp32-elf",
        sha256 = "e6d47c1dbd8c8cbfe37271e5e2aac53ee88c9e347ae937e22bf0c73f530efbdf",
    )

    return {
        "cpu": "esp32_xtensa_lx6",
        "compiler": "gcc",
        "builtin_include_directories": [
            "xtensa-esp32-elf/include/c++/8.2.0/xtensa-esp32-elf".format(package_name),
            "xtensa-esp32-elf/include/c++/8.2.0".format(package_name),
            "xtensa-esp32-elf/include".format(package_name),
            "lib/gcc/xtensa-esp32-elf/8.2.0/include".format(package_name),
            "lib/gcc/xtensa-esp32-elf/8.2.0/include-fixed".format(package_name),
        ],
        "system_directories": [
            "external/{}/xtensa-esp32-elf/include/c++/8.2.0/xtensa-esp32-elf".format(package_name),
            "external/{}/xtensa-esp32-elf/include/c++/8.2.0".format(package_name),
            "external/{}/xtensa-esp32-elf/sys-include".format(package_name),
            "external/{}/lib/gcc/xtensa-esp32-elf/8.2.0/include".format(package_name),
            "external/{}/lib/gcc/xtensa-esp32-elf/8.2.0/include-fixed".format(package_name),
        ],
        "linker_dirs": [
            "external/{}/xtensa-esp32-elf/lib".format(package_name),
        ],
        "compile_flags": [
            # C++17
            "-std=c++17",
            # Add debug symbols, will be removed at the postprocessing stage
            "-g",

            # Warnings
            "-Wall",
            "-Wextra",
            "-Werror",

            # Optimization
            "-Os",

            # Do not link or re-order inclusion files
            "-nostdinc",
            "-nostdinc++",
            "--no-standard-includes",

            # Removal of unused code and data at link time
            "-ffunction-sections",
            "-fdata-sections",
            "-mlongcalls",
        ] + COPTS_GCC,
        "link_flags": LINKOPTS_GCC + [
            # No standard libraries
            "-nostdlib",

            # Link little-endian objects
            "-Wl,-EL",

            # Garbage collection
            "-Wl,--gc-sections",

            # Do not link with shared libraries
            "-Wl,-static",
            "-Wl,-Map=output.map",
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
        "bin_cc": "external/{}/bin/xtensa-esp32-elf-gcc".format(package_name),
        "bin_cpp": "external/{}/bin/xtensa-esp32-elf-gcc".format(package_name),
        "bin_cov": "external/{}/bin/xtensa-esp32-elf-gcov".format(package_name),
        "bin_objdump": "external/{}/bin/xtensa-esp32-elf-objdump".format(package_name),
        "bin_ld": "external/{}/bin/xtensa-esp32-elf-gcc".format(package_name),
        "bin_strip": "external/{}/bin/xtensa-esp32-elf-strip".format(package_name),
    }
