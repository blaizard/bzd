"""Metadata for Clang toolchains."""

def linux(_module_ctx, _name):
    """Metadata for Linux toolchains.

    Args:
        _module_ctx: The module context.
        _name: The name of the toolchain.

    Returns:
        The toolchain metadata.
    """

    return {
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
            "@bzd_toolchain_cc//:fragments/clang/linux/linux.BUILD",
        ],
        "loads": {
            "@bzd_utils//:sh_binary_wrapper.bzl": ["sh_binary_wrapper"],
        },
        "tools": {
            "clang_format": "clang_format",
            "clang_tidy": "clang_tidy",
            "llvm_symbolizer": "llvm_symbolizer",
        },
    }
