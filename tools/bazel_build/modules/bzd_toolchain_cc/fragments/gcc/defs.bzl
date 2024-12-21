"""Metadata for GCC toolchains."""

load("//:fragments/gcc/linux_13.1.0/defs.bzl", "linux_x86_64_13_1_0")
load("//:fragments/gcc/linux_14.2.0/defs.bzl", "linux_x86_64_14_2_0")

gcc = {
    "gcc_13.1.0": {
        "linux-x86_64": linux_x86_64_13_1_0,
    },
    "gcc_14.2.0": {
        "linux-x86_64": linux_x86_64_14_2_0,
    },
}
