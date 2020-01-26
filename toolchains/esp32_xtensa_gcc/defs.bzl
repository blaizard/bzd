load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("//tools/bazel.build/toolchains:defs.bzl", "toolchain_maker")

def _load_esp32_xtensa_gcc_8_2_0(name):

    # Compiler
    clang_package_name = "esp32_xtensa_gcc_8_2_0"
    http_archive(
        name = clang_package_name,
        build_file = "//toolchains/esp32_xtensa_gcc:{}.BUILD".format(clang_package_name),
        urls = [
            "https://dl.espressif.com/dl/xtensa-esp32-elf-gcc8_2_0-esp-2019r2-linux-amd64.tar.gz",
        ],
        strip_prefix = "xtensa-esp32-elf",
        sha256 = "e6d47c1dbd8c8cbfe37271e5e2aac53ee88c9e347ae937e22bf0c73f530efbdf",
    )

    # SDK
    sdk_package_name = "esp32_xtensa_sdk_2020.01.14"
    http_archive(
        name = sdk_package_name,
        build_file = "//toolchains/esp32_xtensa_gcc:{}.BUILD".format(sdk_package_name),
        urls = [
            "https://github.com/blaizard/bzd-esp32-xtensa-sdk/archive/master.zip",
        ],
        strip_prefix = "bzd-esp32-xtensa-sdk-master",
        sha256 = "fd5f17a1fbf6891a3e48d85a4acf170044c9c4931e5184a7661611f80ae13cb6",
    )

    toolchain_maker(
        name = name,
        cpu = "esp32_xtensa",
        compiler = "gcc",
        platforms = [
            "@//tools/bazel.build/platforms:esp32_xtensa",
        ],
        exec_compatible_with = [
            "@//tools/bazel.build/platforms/al:linux",
            "@//tools/bazel.build/platforms/isa:xtensa",
        ],
        target_compatible_with = [
            "@//tools/bazel.build/platforms/al:linux",
            "@//tools/bazel.build/platforms/isa:xtensa",
        ],
        builtin_include_directories = [
            "%package(@{}//)%/xtensa-esp32-elf/include/c++/8.2.0/xtensa-esp32-elf".format(clang_package_name),
            "%package(@{}//)%/xtensa-esp32-elf/include/c++/8.2.0".format(clang_package_name),
            "%package(@{}//)%/xtensa-esp32-elf/include".format(clang_package_name),
            "%package(@{}//)%/lib/gcc/xtensa-esp32-elf/8.2.0/include".format(clang_package_name),
            "%package(@{}//)%/lib/gcc/xtensa-esp32-elf/8.2.0/include-fixed".format(clang_package_name),
        ],
        system_directories = [
            "external/{}/xtensa-esp32-elf/include/c++/8.2.0/xtensa-esp32-elf".format(clang_package_name),
            "external/{}/xtensa-esp32-elf/include/c++/8.2.0".format(clang_package_name),
            "external/{}/xtensa-esp32-elf/sys-include".format(clang_package_name),
            "external/{}/lib/gcc/xtensa-esp32-elf/8.2.0/include".format(clang_package_name),
            "external/{}/lib/gcc/xtensa-esp32-elf/8.2.0/include-fixed".format(clang_package_name),
        ],
        linker_dirs = [
            "external/{}/xtensa-esp32-elf/lib".format(clang_package_name),
            "external/{}/ld".format(sdk_package_name),
            "external/{}/lib".format(sdk_package_name),
        ],
        compile_flags = [
            "-std=c++14",

            # Do not link or re-order inclusion files
            "-nostdinc",
            "-nostdinc++",
            "--no-standard-includes",
            "-nostdlib",

            # Make the compilation deterministic
            #"-fstack-protector",
            #"-fPIE",
            #"-no-canonical-prefixes",

            # Warnings
            "-Wall",
            "-Wno-missing-braces",
            "-Wno-builtin-macro-redefined",

            # Keep stack frames for debugging
            "-fno-omit-frame-pointer",

            # Optimization
            "-O3",

            # Removal of unused code and data at link time
            "-ffunction-sections",
            "-fdata-sections",

            # Use linkstamping instead of these
            "-D__DATE__=\"redacted\"",
            "-D__TIMESTAMP__=\"redacted\"",
            "-D__TIME__=\"redacted\"",

            "-ffunction-sections",
            "-fdata-sections",
            "-fstrict-volatile-bitfields",
            "-mlongcalls",
            "-nostdlib",

            # From ESP-IDF
            "-mlongcalls",
            "-Wno-frame-address",
            "-ffunction-sections",
            "-fdata-sections",
            "-fstrict-volatile-bitfields",
            # warning-related flags
            "-Wall",
            "-Werror=all",
            "-Wno-error=unused-function",
            "-Wno-error=unused-but-set-variable",
            "-Wno-error=unused-variable",
            "-Wno-error=deprecated-declarations",
            "-Wextra",
            "-Wno-unused-parameter",
            "-Wno-sign-compare",
            # always generate debug symbols (even in release mode, these don't
            # go into the final binary so have no impact on size
            "-ggdb",
            "-fno-rtti",
        ],
        link_flags = [
            # Link little-endian objects
            "-Wl,-EL",

            # Garbage collection
            "-Wl,--gc-sections",

            # Do not link with shared libraries
            "-Wl,-static",
        ],
        dynamic_runtime_libs = [
            "@{}//:dynamic_libraries".format(clang_package_name),
        ],
        static_runtime_libs = [
            "@{}//:static_libraries".format(clang_package_name),
        ],
        filegroup_dependencies = [
            "@{}//:includes".format(clang_package_name),
            "@{}//:bin".format(clang_package_name),
            "@{}//:files".format(sdk_package_name),
        ],
        bin_ar = "external/{}/bin/xtensa-esp32-elf-ar".format(clang_package_name),
        bin_as = "external/{}/bin/xtensa-esp32-elf-as".format(clang_package_name),
        bin_cc = "external/{}/bin/xtensa-esp32-elf-gcc".format(clang_package_name),
        bin_cpp = "external/{}/bin/xtensa-esp32-elf-g++".format(clang_package_name),
        bin_gcov = "external/{}/bin/xtensa-esp32-elf-gcov".format(clang_package_name),
        bin_objdump = "external/{}/bin/xtensa-esp32-elf-objdump".format(clang_package_name),
        bin_ld = "external/{}/bin/xtensa-esp32-elf-gcc".format(clang_package_name),
    )

    native.register_toolchains(
        "@{}//:toolchain".format(name),
    )

    native.register_execution_platforms(
        "@{}//:host_platform".format(name),
    )

def load_esp32_xtensa_gcc():
    _load_esp32_xtensa_gcc_8_2_0(name = "esp32_xtensa_gcc")
