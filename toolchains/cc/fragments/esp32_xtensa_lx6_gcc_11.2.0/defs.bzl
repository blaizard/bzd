load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def toolchain_fragment_esp32_xtensa_lx6_gcc():
    package_name = "esp32_xtensa_lx6_gcc_11.2.0"

    http_archive(
        name = package_name,
        build_file = "//toolchains/cc/fragments/esp32_xtensa_lx6_gcc_11.2.0:{}.BUILD".format(package_name),
        urls = [
            "https://github.com/espressif/crosstool-NG/releases/download/esp-2022r1-RC1/xtensa-esp32-elf-gcc11_2_0-esp-2022r1-RC1-linux-amd64.tar.xz",
        ],
        strip_prefix = "xtensa-esp32-elf",
        sha256 = "5da31dfe66ee97c0e940d81e7fac3fc604bb4cbf75294a29e6d5384ae08102dc",
    )

    return {
        "cpu": "esp32_xtensa_lx6",
        "compiler": "gcc",
        "system_directories": [
            "external/{}/xtensa-esp32-elf/include/c++/11.2.0/xtensa-esp32-elf".format(package_name),
            "external/{}/xtensa-esp32-elf/include/c++/11.2.0".format(package_name),
            "external/{}/xtensa-esp32-elf/sys-include".format(package_name),
            "external/{}/lib/gcc/xtensa-esp32-elf/11.2.0/include".format(package_name),
            "external/{}/lib/gcc/xtensa-esp32-elf/11.2.0/include-fixed".format(package_name),
        ],
        "linker_dirs": [
            "external/{}/xtensa-esp32-elf/lib".format(package_name),
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
        "static_runtime_lib": [
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
