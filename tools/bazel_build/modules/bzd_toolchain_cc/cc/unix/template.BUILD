package(default_visibility = ["//visibility:public"])

load("@bzd_toolchain_cc//cc:unix/unix_cc_toolchain_config.bzl", "cc_toolchain_config")
load("@bzd_toolchain_cc//binary:defs.bzl", "binary_toolchain")
%{loads}

# C++ toolchain

filegroup(
    name = "all_files",
    srcs = [
        ":ar_files",
        ":as_files",
        ":compiler_files",
        ":linker_files",
        ":objcopy_files",
        ":strip_files",
        ":dynamic_libraries_files",
        ":static_libraries_files"
    ],
)

filegroup(name = "empty")

cc_toolchain_config(
    name = "cc-toolchain-config",
    cpu = "%{cpu}",
    compiler = "%{compiler}",
    toolchain_identifier = "cc-toolchain-identifier-%{uid}",
    host_system_name = "unknown",
    target_system_name = "unknown",
    target_libc = "unknown",
    abi_version = "unknown",
    abi_libc_version = "unknown",
    cxx_builtin_include_directories = [
        %{builtin_include_directories}
    ],
    tool_paths = {
        "ar": "%{ar}",
        "as": "%{as}",
        "cpp": "%{cpp}",
        "gcc": "%{cc}",
        "gcov": "%{cov}",
        "ld": "%{ld}",
        "llvm-cov": "%{cov}",
        "nm": "%{nm}",
        "objcopy": "%{objcopy}",
        "objdump": "%{objdump}",
        "strip": "%{strip}",
    },
    compile_flags = [
        %{compile_flags}
    ],
    dbg_compile_flags = [
        %{compile_dev_flags}
    ],
    opt_compile_flags = [
        %{compile_prod_flags}
    ],
    cxx_flags = [],
    link_flags = [
        %{link_flags}
    ],
    link_libs = [],
    opt_link_flags = [],
    coverage_compile_flags = [
        %{coverage_compile_flags}
    ],
    coverage_link_flags = [
        %{coverage_link_flags}
    ],
    unfiltered_compile_flags = [],
    supports_start_end_lib = False,
    builtin_sysroot = "%{sysroot}",
)

cc_toolchain(
    name = "cc_toolchain",
    all_files = ":all_files",
    ar_files = ":ar_files",
    as_files = ":as_files",
    compiler_files = ":compiler_files",
    dwp_files = ":empty",
    linker_files = ":linker_files",
    objcopy_files = ":objcopy_files",
    strip_files = ":strip_files",

    dynamic_runtime_lib = ":dynamic_libraries_files",
    static_runtime_lib = ":static_libraries_files",

    toolchain_config = ":cc-toolchain-config",
    toolchain_identifier = "cc-toolchain-identifier-%{uid}",

    supports_param_files = True,
)

# Binary toolchain

binary_toolchain(
    name = "binary_toolchain",
    %{binary_kwargs}
)

# Aliases

%{aliases}

# Create a package with only what is needed
genrule(
    name = "package",
    srcs = [
        ":all_files",
        ":extra_files"
    ],
    outs = ["%{package_name}.tar.xz"],
    tags = ["manual"],
    cmd = """
    tar -cJf $@ --dereference --transform 's/^\\./%{package_name}/' -C "%{repository_path}" .
    """
)

# Toolchain fragments
