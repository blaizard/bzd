"""CC flags."""

_COPTS_COMMON = [
    # Warnings are handled at the repo level, to avoid having warnings for external projects.

    # Add debug symbols, will be removed at the postprocessing stage.
    "-ggdb3",
    # Generate standard-compliant DWARF-2 debug info, this helps to get proper stack traces
    # at least with clang.
    "-gdwarf-2",

    # Do not expand any symbolic links, resolve references to /../
    # or /./, or make the path absolute when generating a relative
    # prefix. This is needed for bazel to properly identify.
    # dependencies.
    "-no-canonical-prefixes",

    # Add color to the output.
    "-fdiagnostics-color=always",

    # Removal of unused code and data at link time.
    "-ffunction-sections",
    "-fdata-sections",
]

_COPTS_COMMON_DEV = [
    # O0 seems to give better stack traces than -Og.
    "-O0",

    # Keep stack frames for debugging.
    "-fno-omit-frame-pointer",

    # Emit extra code to check for buffer overflows.
    "-fstack-protector-all",

    # No inline to improve stack trace.
    "-fno-inline",
]

_COPTS_COMMON_PROD = [
    # Optimize for size. -Os enables all -O2 optimizations except
    # those that often increase code size.
    "-Os",
]

_COPTS_COMMON_COVERAGE = [
    # Disable any optimization to have the most accurate coverage report.
    "-O0",

    # No inline that will confuse the coverage.
    "-fno-inline",
]

COPTS_GCC_DEV = _COPTS_COMMON_DEV
COPTS_GCC_PROD = _COPTS_COMMON_PROD
COPTS_GCC = _COPTS_COMMON + [
    "-fno-canonical-system-headers",
]
COPTS_GCC_COVERAGE = _COPTS_COMMON_COVERAGE + [
    "-fno-inline-small-functions",
    "-fno-default-inline",
    "-fprofile-arcs",
    "-ftest-coverage",
]

COPTS_CLANG_DEV = _COPTS_COMMON_DEV
COPTS_CLANG_PROD = _COPTS_COMMON_PROD
COPTS_CLANG = _COPTS_COMMON + [
    "-flto",
]
COPTS_CLANG_COVERAGE = _COPTS_COMMON_COVERAGE + [
    "-fprofile-instr-generate",
    "-fcoverage-mapping",
    "-fprofile-arcs",
    "-ftest-coverage",
]

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

_LINKOPTS_COMMON_COVERAGE = [
    "-fprofile-arcs",
]

LINKOPTS_GCC = _LINKOPTS_COMMON + [
    "-fno-canonical-system-headers",
]
LINKOPTS_GCC_COVERAGE = _LINKOPTS_COMMON_COVERAGE

LINKOPTS_CLANG = _LINKOPTS_COMMON + [
    "-flto",
]
LINKOPTS_CLANG_COVERAGE = _LINKOPTS_COMMON_COVERAGE
