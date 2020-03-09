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
]

COPTS_GCC = COPTS_COMMON

COPTS_CLANG = COPTS_COMMON

LINKOPTS_COMMON = [
	"-Wl,-as-needed",
    "-Wl,-no-undefined",
]

LINKOPTS_GCC = LINKOPTS_COMMON

LINKOPTS_CLANG = LINKOPTS_COMMON
