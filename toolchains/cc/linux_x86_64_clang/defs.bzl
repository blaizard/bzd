load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("//tools/bazel_build/toolchains/cc:defs.bzl", "toolchain_maker")

BINARIES = {
    "ar": "bin/llvm-ar",
    "as": "bin/llvm-as",
    "cc": "bin/clang",
    "cpp": "bin/clang",
    "cov": "bin/llvm-cov",
    "objcopy": "bin/llvm-objcopy",
    "objdump": "bin/llvm-objdump",
    "ld": "bin/clang++",
    "strip": "bin/llvm-strip",
}

def _load_linux_x86_64_clang_14_0_6(name):
    # Load dependencies
    package_name = "linux_x86_64_clang_14_0_6"
    http_archive(
        name = package_name,
        build_file = "//toolchains/cc/linux_x86_64_clang:{}.BUILD".format(package_name),
        urls = [
            "http://data.blaizard.com/file/bzd/toolchains/cc/clang/linux_x86_64/linux_x86_64_14.0.6.tar.xz",
        ],
        strip_prefix = "linux_x86_64_14.0.6",
        sha256 = "f39ed3fe655eded1c5765ad0a6375dd7aa8aa6613c63c7a233bc0533fc63374e",
    )

    toolchain_definition = {
        "default": True,
        "cpu": "linux_x86_64",
        "compiler": "clang",
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
            "external/{}/include/c++/v1".format(package_name),
            "external/{}/lib/clang/14.0.6/include".format(package_name),
            "external/{}/include/x86_64-unknown-linux-gnu/c++/v1/".format(package_name),
            "/usr/include/x86_64-linux-gnu",
            "/usr/include",
        ],
        "linker_dirs": [
            "external/{}/lib".format(package_name),
        ],
        "compile_flags": [

            # Use C++20
            "-std=c++20",

            # Standard includes, this is needed to avoid indefined include complain from Bazel.
            "-nostdinc",
            "--no-standard-includes",
        ],
        "link_flags": [
            # Use the lld llvm linker.
            "-fuse-ld=lld",
            "-Wl,--disable-new-dtags",
            "-rdynamic",
            "-stdlib=libc++",
            "-lm",
            "-lc++",
            "-lc++abi",
            "-Wl,-z,relro,-z,now",
            "-no-canonical-prefixes",

            # Stamp the binary with a unique identifier
            "-Wl,--build-id=md5",
            "-Wl,--hash-style=gnu",
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
        "app_executors": {
            "@//toolchains/cc/linux_x86_64_clang:executor_host": "default,host",
        },
    }

    toolchain_maker(
        name = name,
        implementation = "linux_clang",
        definition = toolchain_definition,
    )

def load_linux_x86_64_clang():
    _load_linux_x86_64_clang_14_0_6(name = "linux_x86_64_clang")
