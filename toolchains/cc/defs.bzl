_COPTS_COMMON = [
    # Warnings.
    "-Wall",
    "-Werror",
    "-Wextra",
    "-Wpedantic",
    "-Wvla",
    "-Wextra-semi",
    "-Wnull-dereference",
    "-Wswitch-enum",

    # Add debug symbols, will be removed at the postprocessing stage.
    "-g",

    # Do not expand any symbolic links, resolve references to /../
    # or /./, or make the path absolute when generating a relative
    # prefix. This is needed for bazel to properly identify.
    # dependencies.
    "-no-canonical-prefixes",

    # Removal of unused code and data at link time.
    "-ffunction-sections",
    "-fdata-sections",
]

_COPTS_COMMON_DEV = [
    # Optimize debugging experience. -Og should be the optimization
    # level of choice for the standard edit-compile-debug cycle.
    "-Og",

    # Keep stack frames for debugging.
    "-fno-omit-frame-pointer",

    # Emit extra code to check for buffer overflows.
    "-fstack-protector-all",
]

_COPTS_COMMON_PROD = [
    # Optimize for size. -Os enables all -O2 optimizations except
    # those that often increase code size.
    "-Os",
]

COPTS_GCC_DEV = _COPTS_COMMON_DEV
COPTS_GCC_PROD = _COPTS_COMMON_PROD
COPTS_GCC = _COPTS_COMMON

COPTS_CLANG_DEV = _COPTS_COMMON_DEV
COPTS_CLANG_PROD = _COPTS_COMMON_PROD
COPTS_CLANG = _COPTS_COMMON

_LINKOPTS_COMMON = [
    # Removes unused shared libraries
    #"-Wl,-as-needed",
    # Raise a compilation error if a symbol is undefined during linking
    #"-Wl,-no-undefined",
    "-Wl,-no-as-needed",

    # Do not expand any symbolic links, resolve references to /../
    # or /./, or make the path absolute when generating a relative
    # prefix. This is needed for bazel to properly identify.
    # dependencies.
    "-no-canonical-prefixes",

    # Use garbage collection.
    "-Wl,--gc-sections",
]

LINKOPTS_GCC = _LINKOPTS_COMMON

LINKOPTS_CLANG = _LINKOPTS_COMMON
