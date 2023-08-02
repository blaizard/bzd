"""Metadata for Linux toolchains."""

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
            "@bzd_toolchain_cc//cc:executor_host": "default,host",
        },
        "binaries": {
            "ar": "bin/ar",
            "as": "bin/as",
            "cc": "bin/g++",
            "cov": "bin/gcov",
            "cpp": "bin/cpp",
            "ld": "bin/ld",
            "objcopy": "bin/objcopy",
            "objdump": "bin/objdump",
            "strip": "bin/strip",
        },
        "build_files": [
            "@bzd_toolchain_cc//:fragments/gcc/linux/linux.BUILD",
        ],
    }
