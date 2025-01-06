"""Metadata for Linux toolchains."""

def linux():
    """Metadata for Linux toolchains.

    Returns:
        The toolchain metadata.
    """

    return None, {
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
            "@bzd_toolchain_cc//toolchains/gcc:linux/linux.BUILD",
        ],
    }
