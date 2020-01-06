package(default_visibility = ["//visibility:public"])

load("@bazel_tools//tools/cpp:unix_cc_toolchain_config.bzl", "cc_toolchain_config")

filegroup(
    name = "wrappers",
    srcs = glob([
		"bin/wrapper-*"
	]),
)

filegroup(
    name = "all",
    srcs = [
        ":wrappers",
        %{filegroup_dependencies}
    ],
)

filegroup(
    name = "dynamic_runtime_libs",
    srcs = [
        %{dynamic_runtime_libs}
    ],
)

filegroup(
    name = "static_runtime_libs",
    srcs = [
        %{static_runtime_libs}
    ],
)

filegroup(name = "empty")

cc_toolchain_config(
    name = "cc-toolchain-config",
    cpu = "%{cpu}",
    compiler = "%{compiler}",
    toolchain_identifier = "cc-toolchain-identifier-%{cpu}",
    host_system_name = "i686-unknown-linux-gnu",
    target_system_name = "local",
    target_libc = "glibc_unknown",
    abi_version = "local",
    abi_libc_version = "local",
    cxx_builtin_include_directories = [
        %{builtin_include_directories}
    ],
    tool_paths = {
        "gcc": "bin/wrapper-cc",
        "cpp": "bin/wrapper-cpp",
        "ar": "bin/wrapper-ar",
        "nm": "bin/wrapper-nm",
        "ld": "bin/wrapper-ld",
        "as": "bin/wrapper-as",
        "objcopy": "bin/wrapper-objcopy",
        "gcov": "bin/wrapper-gcov",
        "objdump": "bin/wrapper-objdump",
        "strip": "bin/wrapper-strip",
    },
    compile_flags = [
        %{compile_flags}
    ],
    dbg_compile_flags = [],
    opt_compile_flags = [],
    cxx_flags = [],
    link_flags = [
        %{link_flags}
    ],
    link_libs = [],
    opt_link_flags = [],
    unfiltered_compile_flags = [],
    supports_start_end_lib = False,
)

cc_toolchain(
    name = "cc_toolchain",
    all_files = ":all",
    ar_files = ":all",
    as_files = ":all",
    compiler_files = ":all",
    dwp_files = ":all",
    linker_files = ":all",
    objcopy_files = ":all",
    strip_files = ":all",

    dynamic_runtime_lib = ":dynamic_runtime_libs",
    static_runtime_lib = ":static_runtime_libs",

    toolchain_config = ":cc-toolchain-config",
    toolchain_identifier = "cc-toolchain-identifier-%{cpu}",
)

toolchain(
    name = "toolchain",
    exec_compatible_with = [
        %{exec_compatible_with}
    ],
    target_compatible_with = [
        %{target_compatible_with}
    ],
    toolchain = ":cc_toolchain",
    toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
)
