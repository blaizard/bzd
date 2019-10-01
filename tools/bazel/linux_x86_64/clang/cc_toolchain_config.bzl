load("@bazel_tools//tools/cpp:cc_toolchain_config_lib.bzl", "feature", "flag_group", "flag_set", "tool_path")
load("@bazel_tools//tools/build_defs/cc:action_names.bzl", "ACTION_NAMES")

sysroot = "/opt/toolchains/linux_x86_64"

def _impl(ctx):
    tool_paths = [
        tool_path(
            name = "as",
            path = "/usr/bin/as",
        ),
        tool_path(
            name = "ar",
            path = "/usr/bin/ar",
        ),
        tool_path(
            name = "ld",
            path = "/usr/bin/ld",
        ),
        tool_path(
            name = "cpp",
            path = "/usr/bin/cpp",
        ),
        tool_path(
            name = "gcc",
            path = sysroot + "/clang-7.0.0/bin/clang",
        ),
        tool_path(
            name = "dwp",
            path = "/usr/bin/dwp",
        ),
        tool_path(
            name = "gcov",
            path = "/usr/bin/gcov",
        ),
        tool_path(
            name = "nm",
            path = "/usr/bin/nm",
        ),
        tool_path(
            name = "objcopy",
            path = "/usr/bin/objcopy",
        ),
        tool_path(
            name = "objdump",
            path = "/usr/bin/objdump",
        ),
        tool_path(
            name = "strip",
            path = "/usr/bin/strip",
        ),
    ]

    feature_compile = feature(
        name = "compile",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [
                    ACTION_NAMES.c_compile,
                    ACTION_NAMES.cpp_compile,
                    ACTION_NAMES.cpp_header_parsing,
                    ACTION_NAMES.cpp_module_compile,
                    ACTION_NAMES.cpp_module_codegen,
                ],
                flag_groups = [
                    flag_group(
                        flags = [
                            "-std=c++14",
                        ],
                    ),
                    flag_group(
                        flags = [
                            "-g0",
                            "-O3",
                            "-D",
                            "NDEBUG",
                            "-ffunction-sections",
                            "-fdata-sections",
                        ],
                    ),
                    flag_group(
                        flags = [
                            "-Wno-builtin-macro-redefined",
                            "-D",
                            "__DATE__=\"redacted\"",
                            "-D",
                            "__TIMESTAMP__=\"redacted\"",
                            "-D",
                            "__TIME__=\"redacted\"",
                        ],
                    ),
                    flag_group(
                        flags = [
                            "-fstack-protector",
                            "-Wall",
                            "-B/usr/bin",
                            "-fno-omit-frame-pointer",
                        ],
                    ),
                    flag_group(
                        iterate_over = "include_paths",
                        flags = ["-I", "%{include_paths}"],
                    ),
                    flag_group(
                        iterate_over = "quote_include_paths",
                        flags = ["-iquote", "%{quote_include_paths}"],
                    ),
                    flag_group(
                        iterate_over = "system_include_paths",
                        flags = [
                            "-isystem", "%{system_include_paths}",
                            "-isystem", "/opt/toolchains/linux_x86_64/glibc-2.30",
                            "-isystem", "/opt/toolchains/linux_x86_64/glibc-2.30/include",
                            "-isystem", "/opt/toolchains/linux_x86_64/clang-7.0.0/include/c++/v1",
                            "-isystem", "/opt/toolchains/linux_x86_64/clang-7.0.0/lib/clang/7.0.0/include",
                        ],
                    ),
                ],
            ),
        ],
    )

    feature_linker = feature(
        name = "linker",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [
                    ACTION_NAMES.cpp_link_executable,
                    ACTION_NAMES.cpp_link_dynamic_library,
                    ACTION_NAMES.cpp_link_nodeps_dynamic_library,
                    ACTION_NAMES.lto_backend,
                ],
                flag_groups = [
                    flag_group(
                        flags = [
                            "-fuse-ld=gold",
                            "-Wl,-no-as-needed",
                            "-Wl,-z,relro,-z,now",
                            "-lstdc++",
                            "-lm",
                        ],
                    ),
                ],
            ),
        ],
    )

    return cc_common.create_cc_toolchain_config_info(
        ctx = ctx,
        toolchain_identifier = "linux_x86_64/clang",
        host_system_name = "linux_x86_64",
        target_system_name = "linux_x86_64",
        target_cpu = "linux_x86_64",
        target_libc = "unknown",
        compiler = "clang",
        abi_version = "unknown",
        abi_libc_version = "unknown",
        tool_paths = tool_paths,
        features = [feature_compile, feature_linker],
        builtin_sysroot = "/opt/toolchains/linux_x86_64",
        cxx_builtin_include_directories = ["/opt/toolchains/linux_x86_64"],
    )

cc_toolchain_config = rule(
    implementation = _impl,
    attrs = {},
    provides = [CcToolchainConfigInfo],
)
