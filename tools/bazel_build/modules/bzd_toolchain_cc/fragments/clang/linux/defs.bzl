def linux(module_ctx, name):

    _ = module_ctx
    _ = name

    return {
        "build_files": [
            "@bzd_toolchain_cc//:fragments/clang/linux/linux.BUILD",
        ],
        "binaries": {
            "ar": "bin/llvm-ar",
            "as": "bin/llvm-as",
            "cc": "bin/clang",
            "cpp": "bin/clang",
            "cov": "bin/llvm-cov",
            "objcopy": "bin/llvm-objcopy",
            "objdump": "bin/llvm-objdump",
            "ld": "bin/clang++",
            "strip": "bin/llvm-strip",
        },
        "tools": {
            "clang_format": "clang_format",
            "clang_tidy": "clang_tidy",
            "llvm_symbolizer": "llvm_symbolizer",
        },
        "app_executors": {
            ":executor_host": "default,host",
        },
        "loads": {
            "@bzd_utils//:sh_binary_wrapper.bzl": ["sh_binary_wrapper"]
        }
    }
