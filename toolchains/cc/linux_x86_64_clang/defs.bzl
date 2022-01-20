load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("//tools/bazel_build/toolchains/cc:defs.bzl", "toolchain_maker")

def _load_linux_x86_64_clang_12_0_0(name):
    # Load dependencies
    package_name = "linux_x86_64_clang_12_0_0"
    http_archive(
        name = package_name,
        build_file = "//toolchains/cc/linux_x86_64_clang:{}.BUILD".format(package_name),
        urls = [
            "https://github.com/llvm/llvm-project/releases/download/llvmorg-12.0.0/clang+llvm-12.0.0-x86_64-linux-gnu-ubuntu-20.04.tar.xz",
        ],
        strip_prefix = "clang+llvm-12.0.0-x86_64-linux-gnu-ubuntu-20.04",
        sha256 = "a9ff205eb0b73ca7c86afc6432eed1c2d49133bd0d49e47b15be59bbf0dd292e",
    )

    toolchain_definition = {
        "cpu": "linux_x86_64",
        "compiler": "clang",
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
            "include/c++/v1".format(package_name),
            "lib/clang/12.0.0/include".format(package_name),
            "/usr/include/x86_64-linux-gnu",
            "/usr/include",
        ],
        "system_directories": [
            "external/{}/include/c++/v1".format(package_name),
            "external/{}/lib/clang/12.0.0/include".format(package_name),
            "/usr/include/x86_64-linux-gnu",
            "/usr/include",
        ],
        "linker_dirs": [
            "external/{}/lib".format(package_name),
        ],
        "compile_flags": [

            # Use incomplete C++20
            "-std=c++20",

            # Standard includes, this is needed to avoid indefined include complain from Bazel.
            "-nostdinc",
            "--no-standard-includes",

            # From migration to 9.00 -> 12.0.0
            "-Wno-non-c-typedef-for-linkage",

            # Add support to coroutines.
            "-Wno-unused-command-line-argument",
            "-stdlib=libc++",
            "-fcoroutines-ts",
        ],
        "link_flags": [
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
            "-Wl,-rpath=%{{absolute_external}}/{}/lib".format(package_name),

            # Stamp the binary with a unique identifier
            "-Wl,--build-id=md5",
            "-Wl,--hash-style=gnu",
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
        "bin_ar": "external/{}/bin/llvm-ar".format(package_name),
        "bin_as": "external/{}/bin/llvm-as".format(package_name),
        "bin_cc": "external/{}/bin/clang".format(package_name),
        "bin_cpp": "external/{}/bin/clang".format(package_name),
        "bin_cov": "external/{}/bin/llvm-cov".format(package_name),
        "bin_objdump": "external/{}/bin/llvm-objdump".format(package_name),
        "bin_ld": "external/{}/bin/clang++".format(package_name),
        "bin_strip": "external/{}/bin/llvm-strip".format(package_name),
        "app_executors": {
            "@//tools/bazel_build/toolchains/cc:executor_host": "default,host",
        },
    }

    toolchain_maker(
        name = name,
        implementation = "linux_clang",
        definition = toolchain_definition,
    )

def load_linux_x86_64_clang():
    _load_linux_x86_64_clang_12_0_0(name = "linux_x86_64_clang")
