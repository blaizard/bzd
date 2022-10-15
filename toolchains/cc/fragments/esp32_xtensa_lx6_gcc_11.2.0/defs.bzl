load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

BINARIES = {
    "ar": "xtensa-esp32-elf/bin/ar",
    "as": "xtensa-esp32-elf/bin/as",
    "cc": "bin/xtensa-esp32-elf-gcc",
    "cpp": "bin/xtensa-esp32-elf-gcc",
    "cov": "bin/xtensa-esp32-elf-gcov",
    "objcopy": "xtensa-esp32-elf/bin/objcopy",
    "objdump": "xtensa-esp32-elf/bin/objdump",
    "ld": "xtensa-esp32-elf/bin/ld",
    "strip": "xtensa-esp32-elf/bin/strip",
}

def toolchain_fragment_esp32_xtensa_lx6_gcc():
    package_name = "esp32_xtensa_lx6_gcc_11.2.0"

    http_archive(
        name = package_name,
        build_file = "//toolchains/cc/fragments/esp32_xtensa_lx6_gcc_11.2.0:{}.BUILD".format(package_name),
        urls = [
            "http://data.blaizard.com/file/bzd/toolchains/cc/gcc/esp32_xtensa_lx6/xtensa-esp32-elf-gcc11_2_0-esp-2022r1-linux-amd64.tar.xz",
        ],
        strip_prefix = "xtensa-esp32-elf",
        sha256 = "698d8407e18275d18feb7d1afdb68800b97904fbe39080422fb8609afa49df30",
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
        "ar_files": ["@{}//:ar_files".format(package_name)],
        "as_files": ["@{}//:as_files".format(package_name)],
        "compiler_files": ["@{}//:compiler_files".format(package_name)],
        "linker_files": ["@{}//:linker_files".format(package_name)],
        "objcopy_files": ["@{}//:objcopy_files".format(package_name)],
        "strip_files": ["@{}//:strip_files".format(package_name)],
        "dynamic_libraries_files": ["@{}//:dynamic_libraries_files".format(package_name)],
        "static_libraries_files": ["@{}//:static_libraries_files".format(package_name)],
        "binaries": {k: "external/{}/{}".format(package_name, v) for k, v in BINARIES.items()},
    }
