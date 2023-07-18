load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("//tools/bazel_build/toolchains/cc:defs.bzl", "toolchain_maker")

BINARIES = {
    "ar": "bin/ar",
    "as": "bin/as",
    "cc": "bin/g++",
    "cpp": "bin/cpp",
    "cov": "bin/gcov",
    "objcopy": "bin/objcopy",
    "objdump": "bin/objdump",
    "ld": "bin/ld",
    "strip": "bin/strip",
}

def _load_linux_x86_64_gcc_13_1_0(name):
    # Load dependencies
    package_name = "linux_x86_64_gcc_13_1_0"
    http_archive(
        name = package_name,
        build_file = "//toolchains/cc/linux_x86_64_gcc:{}.BUILD".format(package_name),
        urls = [
            "http://data.blaizard.com/file/bzd/toolchains/cc/gcc/linux_x86_64/linux_x86_64_13.1.0.tar.xz",
        ],
        strip_prefix = "linux_x86_64_13.1.0",
        sha256 = "07f2855d072cb7de1ff1d81612d1d41cf6e548c20f4c16a09d268086e5690166",
    )

    toolchain_definition = {
        "cpu": "linux_x86_64",
        "compiler": "gcc",
        "exec_compatible_with": [
            "@platforms//os:linux",
            "@platforms//cpu:x86_64",
        ],
        "target_compatible_with": [
            "@platforms//os:linux",
            "@platforms//cpu:x86_64",
        ],
        "builtin_include_directories": [
            "/usr/include/x86_64-linux-gnu",
            "/usr/include",
        ],
        "system_directories": [
            "external/{}/include/c++/13.1.0/x86_64-pc-linux-gnu".format(package_name),
            "external/{}/include/c++/13.1.0".format(package_name),
            "external/{}/lib/gcc/x86_64-pc-linux-gnu/13.1.0/include".format(package_name),
            "external/{}/lib/gcc/x86_64-pc-linux-gnu/13.1.0/include-fixed".format(package_name),
            "/usr/include/x86_64-linux-gnu",
            "/usr/include",
        ],
        "linker_dirs": [
            "external/{}/lib/gcc/x86_64-pc-linux-gnu/13.1.0".format(package_name),
            "external/{}/lib64".format(package_name),
        ],
        "compile_flags": [

            # Use C++20.
            "-std=c++20",
            "-fcoroutines",

            # There is a bug with coroutines
            "-Wno-unused-value",
        ],
        "link_flags": [
            "-Wl,--disable-new-dtags",
            "-rdynamic",
            "-Wl,-z,relro,-z,now",
            # Link statically with the provided libstdc++.
            # This is to avoid relying on a different libstdc++ at runtime, which improves reproductibility.
            "-static-libstdc++",
            "-lm",
        ],
        "all_files": ["@{}//:{}".format(package_name, k) for k in BINARIES.keys()],
        "ar_files": ["@{}//:ar_files".format(package_name)],
        "as_files": ["@{}//:as_files".format(package_name)],
        "compiler_files": ["@{}//:compiler_files".format(package_name)],
        "linker_files": ["@{}//:linker_files".format(package_name)],
        "objcopy_files": ["@{}//:objcopy_files".format(package_name)],
        "strip_files": ["@{}//:strip_files".format(package_name)],
        "dynamic_libraries_files": ["@{}//:dynamic_libraries_files".format(package_name)],
        "static_libraries_files": ["@{}//:static_libraries_files".format(package_name)],
        "binaries": {k: "external/{}/{}".format(package_name, v) for k, v in BINARIES.items()},
        "app_executors": {
            "@//tools/bazel_build/toolchains/cc:executor_host": "default,host",
        },
    }

    toolchain_maker(
        name = name,
        implementation = "linux_gcc",
        definition = toolchain_definition,
    )

def load_linux_x86_64_gcc():
    _load_linux_x86_64_gcc_13_1_0(name = "linux_x86_64_gcc")
