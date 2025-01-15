"""Metadata for Clang toolchains."""

def linux():
    """Metadata for Linux toolchains.

    Returns:
        The toolchain metadata.
    """

    return None, {
        "app_executors": {
            ":executor_host": "default,host",
        },
        "binaries": {
            "ar": "bin/llvm-ar",
            "as": "bin/llvm-as",
            "cc": "bin/clang",
            "cov": "bin/llvm-cov",
            "cpp": "bin/clang",
            "ld": "bin/clang++",
            "objcopy": "bin/llvm-objcopy",
            "objdump": "bin/llvm-objdump",
            "strip": "bin/llvm-strip",
        },
        "build_files": [
            "@bzd_toolchain_cc//toolchains/clang:linux/linux.BUILD",
        ],
        "loads": {
            "@bzd_lib//:sh_binary_wrapper.bzl": ["sh_binary_wrapper"],
        },
        "tools": {
            "clang_format": "clang_format",
            "clang_tidy": "clang_tidy",
            "llvm_symbolizer": "llvm_symbolizer",
            "package": "package",
        },
    }
