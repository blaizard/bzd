load("//tools/bazel.build/toolchains/cc:defs.bzl", "app_toolchain")

app_toolchain(
    name = "app",
)

toolchain(
    name = "app_toolchain",
    exec_compatible_with = [
        "@platforms//os:linux",
    ],
    target_compatible_with = [
        "@platforms//os:linux",
    ],
    toolchain = ":app",
    toolchain_type = "//tools/bazel.build/toolchains/cc:toolchain_type",
)
