def linux(module_ctx, name):
    _ = module_ctx
    _ = name

    return {
        "build_files": [
            "@bzd_toolchain_cc//:fragments/gcc/linux/linux.BUILD",
        ],
        "binaries": {
            "ar": "bin/ar",
            "as": "bin/as",
            "cc": "bin/g++",
            "cpp": "bin/cpp",
            "cov": "bin/gcov",
            "objcopy": "bin/objcopy",
            "objdump": "bin/objdump",
            "ld": "bin/ld",
            "strip": "bin/strip",
        },
        "app_executors": {
            "@bzd_toolchain_cc//cc:executor_host": "default,host",
        },
    }
