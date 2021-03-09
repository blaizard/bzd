load("//tools/bazel_build/toolchains/cc:defs.bzl", "toolchain_maker")
load("//toolchains/cc:defs.bzl", "COPTS_GCC", "LINKOPTS_GCC")

def _load_linux_x86_64_gcc_10_2_0(name):
    # Load dependencies
    package_name = "linux_x86_64_gcc_10_2_0"

    toolchain_definition = {
        "cpu": "linux_x86_64",
        "compiler": "gcc",
        "docker_image": "docker://docker.io/blaizard/linux_x86_64_gcc:10.2.0",
        "host_platforms": [
            "@//tools/bazel_build/platforms:linux_x86_64",
        ],
        "platforms": [
            "@//tools/bazel_build/platforms:linux_x86_64",
        ],
        "exec_compatible_with": [
            "@//tools/bazel_build/platforms/al:linux",
            "@//tools/bazel_build/platforms/isa:x86_64",
        ],
        "target_compatible_with": [
            "@//tools/bazel_build/platforms/al:linux",
            "@//tools/bazel_build/platforms/isa:x86_64",
        ],
        "builtin_include_directories": [
            "/usr/lib/gcc/x86_64-linux-gnu/10/include",
            "/usr/include/c++/10",
            "/usr/include/x86_64-linux-gnu/c++/10",
            "/usr/lib/gcc/x86_64-linux-gnu/10/include",
            "/usr/lib/gcc/x86_64-linux-gnu/10/include-fixed",
            "/usr/include/x86_64-linux-gnu",
            "/usr/include",
        ],
        "system_directories": [
            "/usr/include/c++/10",
            "/usr/include/x86_64-linux-gnu/c++/10",
            "/usr/lib/gcc/x86_64-linux-gnu/10/include",
            "/usr/lib/gcc/x86_64-linux-gnu/10/include-fixed",
            "/usr/include/x86_64-linux-gnu",
            "/usr/include",
        ],
        "linker_dirs": [
            "/usr/lib/gcc/x86_64-linux-gnu/10",
            "/usr/lib/x86_64-linux-gnu",
        ],
        "compile_flags": [
            # Do not link or re-order inclusion files
            #"-nostdinc++",
            #"-nostdinc",
            "--no-standard-includes",

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

            "-Wno-stringop-truncation",

            # Optimization
            "-O3",

            # Removal of unused code and data at link time
            "-ffunction-sections",
            "-fdata-sections",

            # Use linkstamping instead of these
            "-D__DATE__=\"redacted\"",
            "-D__TIMESTAMP__=\"redacted\"",
            "-D__TIME__=\"redacted\"",
        ] + COPTS_GCC,
        "link_flags": LINKOPTS_GCC + [
            "-Wl,--disable-new-dtags",
            "-Wl,--gc-sections",
            "-rdynamic",
            "-Wl,-z,relro,-z,now",
            "-no-canonical-prefixes",
            #"-Wl,-rpath=%{{absolute_external}}/{}/usr/lib/gcc/x86_64-linux-gnu/8".format(package_name),

            # Stamp the binary with a unique identifier
            "-Wl,--build-id=md5",
            "-Wl,--hash-style=gnu",

            "-lstdc++",
        ],
        "coverage_compile_flags": [
            "-fprofile-instr-generate",
            "-fcoverage-mapping",
            "-fprofile-arcs",
            "-ftest-coverage",
            "-fno-inline",
            "-O0",
        ],
        "coverage_link_flags": [
            "-fprofile-instr-generate",
            "-fcoverage-mapping",
            "-fprofile-arcs",
        ],
        "dynamic_runtime_libs": [
            #"@{}//:dynamic_libraries".format(package_name),
        ],
        "static_runtime_libs": [
            #"@{}//:static_libraries".format(package_name),
        ],
        "filegroup_dependencies": [
            #"@{}//:includes".format(package_name),
            #"@{}//:bin".format(package_name),
        ],
        "bin_ar": "/usr/bin/x86_64-linux-gnu-ar",
        "bin_as": "/usr/bin/x86_64-linux-gnu-as",
        "bin_cc": "/usr/bin/x86_64-linux-gnu-gcc-10",
        "bin_cpp": "/usr/bin/x86_64-linux-gnu-cpp-10",
        "bin_cov": "/usr/bin/x86_64-linux-gnu-gcov-10",
        "bin_objdump": "/usr/bin/x86_64-linux-gnu-objdump",
        "bin_ld": "/usr/bin/x86_64-linux-gnu-ld",
        "bin_strip": "/usr/bin/x86_64-linux-gnu-strip",
    }

    toolchain_maker(
        name = name,
        implementation = "linux",
        definition = toolchain_definition,
    )

def load_linux_x86_64_gcc():
    _load_linux_x86_64_gcc_10_2_0(name = "linux_x86_64_gcc")
