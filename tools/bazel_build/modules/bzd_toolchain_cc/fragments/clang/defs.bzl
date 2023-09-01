"""Metadata for Clang toolchains."""

load("@bzd_toolchain_cc//:fragments/clang/linux_arm64_17.0.0/defs.bzl", "linux_arm64_17_0_0")
load("@bzd_toolchain_cc//:fragments/clang/linux_x86_64_17.0.0/defs.bzl", "linux_x86_64_17_0_0")

clang = {
    "clang_17.0.0": {
        "linux-arm64": linux_arm64_17_0_0,
        "linux-x86_64": linux_x86_64_17_0_0,
    },
}
