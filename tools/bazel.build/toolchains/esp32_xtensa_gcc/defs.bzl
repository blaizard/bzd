load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("//tools/bazel.build:toolchain.bzl", "toolchain_maker")

def _load_esp32_xtensa_gcc_5_2_0(name):
    # Load dependencies
    clang_package_name = "esp32_xtensa_gcc_5_2_0"
    http_archive(
        name = clang_package_name,
        build_file = "//tools/bazel.build:toolchains/esp32_xtensa_gcc/{}.BUILD".format(clang_package_name),
        urls = [
            "https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz",
        ],
        strip_prefix = "xtensa-esp32-elf",
        sha256 = "3fe96c151d46c1d4e5edc6ed690851b8e53634041114bad04729bc16b0445156",
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
            "%package(@{}//)%/lib/gcc/xtensa-esp32-elf/5.2.0/include".format(clang_package_name),
            "%package(@{}//)%/xtensa-esp32-elf/include/c++/5.2.0/xtensa-esp32-elf".format(clang_package_name),
            "%package(@{}//)%/xtensa-esp32-elf/include/c++/5.2.0".format(clang_package_name),
            "%package(@{}//)%/xtensa-esp32-elf/include".format(clang_package_name),
            #"/usr/local/include",
            #"/usr/include",
        ],
        system_directories = [
            "external/{}/lib/gcc/xtensa-esp32-elf/5.2.0/include".format(clang_package_name),
            "external/{}/xtensa-esp32-elf/include/c++/5.2.0/xtensa-esp32-elf".format(clang_package_name),
            "external/{}/xtensa-esp32-elf/include/c++/5.2.0".format(clang_package_name),
            "external/{}/xtensa-esp32-elf/include".format(clang_package_name),
            #"/usr/local/include",
            #"/usr/include",
        ],
        linker_dirs = [
            "external/{}/xtensa-esp32-elf/lib".format(clang_package_name),
        ],
        compile_flags = [
            "-std=c++14",
            #"--sysroot=external/esp32_xtensa_gcc_5_2_0/xtensa-esp32-elf/sysroot",

            # Do not link or re-order inclusion files
            "-nostdinc",
            "-nostdinc++",
            "--no-standard-includes",
            "-nostdlib",

            # Make the compilation deterministic
            "-fstack-protector",
            "-fPIE",
            "-no-canonical-prefixes",

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
        ],
        link_flags = [
            "-Wl,-as-needed",
            "-Wl,--disable-new-dtags",
            "-Wl,--gc-sections",
            "-nostdlib",
            "-rdynamic",
            "-lm",
            "-lstdc++",
            "-lc",
            "-Wl,-z,relro,-z,now",
            "-no-canonical-prefixes",

            # Stamp the binary with a unique identifier
            "-Wl,--build-id=md5",
            "-Wl,--hash-style=gnu",
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
        ],
        bin_ar = "external/{}/bin/xtensa-esp32-elf-ar".format(clang_package_name),
        bin_as = "external/{}/bin/xtensa-esp32-elf-as".format(clang_package_name),
        bin_cc = "external/{}/bin/xtensa-esp32-elf-gcc".format(clang_package_name),
        bin_cpp = "external/{}/bin/xtensa-esp32-elf-g++".format(clang_package_name),
        bin_gcov = "external/{}/bin/xtensa-esp32-elf-gcov".format(clang_package_name),
        bin_objdump = "external/{}/bin/xtensa-esp32-elf-objdump".format(clang_package_name),
        bin_ld = "external/{}/bin/xtensa-esp32-elf-ld".format(clang_package_name),
    )

    native.register_toolchains(
        "@{}//:toolchain".format(name),
    )

    native.register_execution_platforms(
        "@{}//:host_platform".format(name),
    )

def load_esp32_xtensa_gcc():
    _load_esp32_xtensa_gcc_5_2_0(name = "esp32_xtensa_gcc")
