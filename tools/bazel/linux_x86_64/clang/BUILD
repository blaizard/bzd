package(default_visibility = ["//visibility:public"])

load(":cc_toolchain_config.bzl", "cc_toolchain_config")

cc_toolchain_config(name = "cc_toolchain_config")

filegroup(
    name = "empty",
    srcs = [],
)

cc_toolchain(
   name = "cc_toolchain",
   toolchain_identifier = "local",
   toolchain_config = ":cc_toolchain_config",
   all_files = ":empty",
   compiler_files = ":empty",
   dwp_files = ":empty",
   linker_files = ":empty",
   objcopy_files = ":empty",
   strip_files = ":empty",
)

toolchain(
    name = "clang",
    toolchain = ":cc_toolchain",
    toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
)
