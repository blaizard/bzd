load("@debian_repository_rules//:debian.bzl", "debian_archive")
load("//tools/bazel_build/toolchains/cc:defs.bzl", "toolchain_maker")
load("//toolchains/cc:defs.bzl", "COPTS_GCC", "LINKOPTS_GCC")

def _load_linux_x86_64_gcc_8_4_0(name):
    # Load dependencies
    package_name = "linux_x86_64_gcc_8_4_0"

    debian_archive(
        name = package_name,
        build_file = "//toolchains/cc/linux_x86_64_gcc:{}.BUILD".format(package_name),
        urls = {
            "http://security.ubuntu.com/ubuntu/pool/universe/g/gcc-8/g++-8_8.4.0-1ubuntu1~19.10_amd64.deb": "edf656e1c0e5fcaaaf0787168b58421bc8e2186dbc679cac5322672d2ef64d4c",
            "http://security.ubuntu.com/ubuntu/pool/universe/g/gcc-8/gcc-8_8.4.0-1ubuntu1~19.10_amd64.deb": "b74dc84c2832b960e0f71754b417a7579d01279e2c5c23454019b7f3d92a3262",
            "http://security.ubuntu.com/ubuntu/pool/universe/g/gcc-8/gcc-8-base_8.4.0-1ubuntu1~19.10_amd64.deb": "e821835430b7316fd83b367c03c2665a73bd56db22d0ca794313f86fe9d5bec9",
            "http://security.ubuntu.com/ubuntu/pool/universe/g/gcc-8/libstdc++-8-dev_8.4.0-1ubuntu1~19.10_amd64.deb": "ff4fa1c4147059f1ec3be33fa6aeebe4a583f1dc667edd5b63ea094ba6bc2449",
            "http://security.ubuntu.com/ubuntu/pool/universe/g/gcc-8/libstdc++6-8-dbg_8.4.0-1ubuntu1~19.10_amd64.deb": "8460c23b978680d01ac4637190950a670fa731561db4ebb625dac9d4b905ffdd",
            "http://security.ubuntu.com/ubuntu/pool/universe/g/gcc-8/cpp-8_8.4.0-1ubuntu1~19.10_amd64.deb": "81d5d561f577c02325f8d22fce8c9b77686a36adf348523b72ca3d977e49c876",
            "http://security.ubuntu.com/ubuntu/pool/main/b/binutils/binutils_2.33-2ubuntu1.2_amd64.deb": "37d73a6970ef18bd7304108befb5300dccb04d3df704bb0c547732c7c0f38fa5",
            "http://de.archive.ubuntu.com/ubuntu/pool/main/i/isl/libisl21_0.21-2_amd64.deb": "87b8db8b8f4011f05fe17407417b03f1d53792f3fdf573c6d28d44bc18b0f96f",

            #"http://security.ubuntu.com/ubuntu/pool/universe/g/gcc-8/gcc-8_8.4.0-1ubuntu1~18.04_amd64.deb": "0d5e1f785fdd0f03638444abbff903b64a61704361d1cd93b47007ab5dd2f97c",
            #"http://security.ubuntu.com/ubuntu/pool/universe/g/gcc-8/cpp-8_8.4.0-1ubuntu1~18.04_amd64.deb": "74a4ca422a0cb9f3d3384d6a6a39439fb8f15994cdda5c1bea567549a9099c7a",
            #"http://security.ubuntu.com/ubuntu/pool/universe/g/gcc-8/g++-8_8.4.0-1ubuntu1~18.04_amd64.deb": "b23142b9951ec4fbd53d27f031ac62ef3f4d99bc3f5a6701b8d94dd46194fd96",
            #"http://security.ubuntu.com/ubuntu/pool/main/b/binutils/binutils_2.34-7ubuntu1_amd64.deb": "d229dafd2f757d6a9ea5ed3b28b9c246990fceeb4831a1dcb2efc657ad357d6a",
            #"http://security.ubuntu.com/ubuntu/pool/main/b/binutils/binutils-x86-64-linux-gnu_2.34-7ubuntu1_amd64.deb": "e968042c53614f39983f56fda865e03b9939b2256350c0dee7ce499eff4aca95",
            #"http://security.ubuntu.com/ubuntu/pool/main/g/gcc-8/libgcc-8-dev_8.4.0-1ubuntu1~18.04_amd64.deb": "5285b3ca6a0cce7ce77bb15072206066a6a6b57e7b7c83a42737cb5cbdc0976d",
            #"http://security.ubuntu.com/ubuntu/pool/universe/g/gcc-8/libstdc++-8-dev_8.4.0-1ubuntu1~18.04_amd64.deb": "5d16e6c1b862052285660a1e426c7f05ac7c74b6ca5579648a2cb73d07916aff",
            #"http://security.ubuntu.com/ubuntu/pool/main/g/gcc-8/libstdc++6_8.4.0-1ubuntu1~18.04_amd64.deb": "4dbd1ffa20011329608f584ff329a10507cfa78c3f76fa34e6f479f9680b64f4",
        },
    )

    toolchain_definition = {
        "cpu": "linux_x86_64",
        "compiler": "gcc",
        "host_platforms": [
            "@//tools/bazel_build/platforms:linux_x86_64",
        ],
        "exec_compatible_with": [
            "@//tools/bazel_build/platforms/al:linux",
            "@//tools/bazel_build/platforms/isa:x86_64",
        ],
        "platforms": [
            "@//tools/bazel_build/platforms:linux_x86_64",
        ],
        "target_compatible_with": [
            "@//tools/bazel_build/platforms/al:linux",
            "@//tools/bazel_build/platforms/isa:x86_64",
        ],
        "builtin_include_directories": [
            "usr/include/c++/8".format(package_name),
            "usr/include/x86_64-linux-gnu/c++/8".format(package_name),
            "usr/lib/gcc/x86_64-linux-gnu/8/include".format(package_name),
            "usr/lib/gcc/x86_64-linux-gnu/8/include-fixed".format(package_name),
            "/usr/include/x86_64-linux-gnu",
            "/usr/include",
        ],
        "system_directories": [
            "external/{}/usr/include/c++/8".format(package_name),
            "external/{}/usr/include/x86_64-linux-gnu/c++/8".format(package_name),
            "external/{}/usr/lib/gcc/x86_64-linux-gnu/8/include".format(package_name),
            "external/{}/usr/lib/gcc/x86_64-linux-gnu/8/include-fixed".format(package_name),
            "/usr/include/x86_64-linux-gnu",
            "/usr/include",
        ],
        "linker_dirs": [
            "external/{}/usr/lib/gcc/x86_64-linux-gnu/8".format(package_name),
            "external/{}/usr/lib/x86_64-linux-gnu".format(package_name),
        ],
        "compile_flags": [
            "-std=c++17",
            # Add debug symbols, will be removed at the postprocessing stage
            "-g",

            # Do not link or re-order inclusion files
            "-nostdinc++",
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
            "-Wno-unused-command-line-argument",

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
        ] + COPTS_GCC,
        "link_flags": LINKOPTS_GCC + [
            "-fuse-ld=lld",
            "-Wl,--disable-new-dtags",
            "-Wl,--gc-sections",
            "-rdynamic",
            "-stdlib=libc++",
            "-lm",
            "-lc++",
            "-lc++abi",
            "-lc",
            "-lgcc_s",
            "-lgcc",
            "-Wl,-z,relro,-z,now",
            "-no-canonical-prefixes",
            "-Wl,-rpath=%{{absolute_external}}/{}/usr/lib/gcc/x86_64-linux-gnu/8".format(package_name),

            # Stamp the binary with a unique identifier
            "-Wl,--build-id=md5",
            "-Wl,--hash-style=gnu",
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
            "@{}//:dynamic_libraries".format(package_name),
        ],
        "static_runtime_libs": [
            "@{}//:static_libraries".format(package_name),
        ],
        "filegroup_dependencies": [
            "@{}//:includes".format(package_name),
            "@{}//:bin".format(package_name),
        ],
        "bin_ar": "external/{}/usr/bin/x86_64-linux-gnu-ar".format(package_name),
        "bin_as": "external/{}/usr/bin/x86_64-linux-gnu-as".format(package_name),
        "bin_cc": "external/{}/usr/bin/x86_64-linux-gnu-gcc-8".format(package_name),
        "bin_cpp": "external/{}/usr/bin/x86_64-linux-gnu-cpp-8".format(package_name),
        "bin_cov": "external/{}/usr/bin/x86_64-linux-gnu-gcov-8".format(package_name),
        "bin_objdump": "external/{}/usr/bin/x86_64-linux-gnu-objdump".format(package_name),
        "bin_ld": "external/{}/usr/bin/x86_64-linux-gnu-ld".format(package_name),
        "bin_strip": "external/{}/usr/bin/x86_64-linux-gnu-strip".format(package_name),
    }

    toolchain_maker(
        name = name,
        implementation = "linux",
        definition = toolchain_definition,
    )

def load_linux_x86_64_gcc():
    _load_linux_x86_64_gcc_8_4_0(name = "linux_x86_64_gcc")
