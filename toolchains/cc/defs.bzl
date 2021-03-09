COPTS_COMMON = [
    "-fdiagnostics-show-template-tree",
    "-Wall",
    "-Werror",
    "-Wextra",
    "-Wpedantic",
    "-Wvla",
    "-Wextra-semi",
    "-Wnull-dereference",
    "-Wswitch-enum",

    # C++17
    "-std=c++17",

    # Add debug symbols, will be removed at the postprocessing stage
    "-g",
]

COPTS_GCC = COPTS_COMMON

COPTS_CLANG = COPTS_COMMON

LINKOPTS_COMMON = [
    # Removes unused shared libraries
    #"-Wl,-as-needed",
    # Raise a compilation error if a symbol is undefined during linking
    #"-Wl,-no-undefined",
    "-Wl,-no-as-needed",
]

LINKOPTS_GCC = LINKOPTS_COMMON

LINKOPTS_CLANG = LINKOPTS_COMMON
