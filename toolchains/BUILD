load("//tools/bazel_build/toolchains/binary:defs.bzl", "binary_toolchain")

binary_toolchain(
    name = "binary",
)

toolchain(
    name = "binary_toolchain",
    exec_compatible_with = [
        "@platforms//os:linux",
    ],
    target_compatible_with = [
        "@platforms//os:linux",
    ],
    toolchain = ":binary",
    toolchain_type = "//tools/bazel_build/toolchains/binary:toolchain_type",
)
