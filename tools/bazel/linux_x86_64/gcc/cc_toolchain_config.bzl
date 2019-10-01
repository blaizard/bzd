load("@bazel_tools//tools/cpp:cc_toolchain_config_lib.bzl", "feature", "flag_group", "flag_set", "tool_path")
load("@bazel_tools//tools/build_defs/cc:action_names.bzl", "ACTION_NAMES")

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
            path = "/usr/bin/gcc",
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
                            "-fno-canonical-system-headers",
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
                            "-Wunused-but-set-parameter",
                            "-Wno-free-nonheap-object",
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
                        flags = ["-isystem", "%{system_include_paths}"],
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
                            "-pass-exit-codes",
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
        toolchain_identifier = "linux_x86_64/gcc",
        host_system_name = "linux_x86_64",
        target_system_name = "linux_x86_64",
        target_cpu = "linux_x86_64",
        target_libc = "unknown",
        compiler = "gcc",
        abi_version = "unknown",
        abi_libc_version = "unknown",
        tool_paths = tool_paths,
        features = [feature_compile, feature_linker],
        cxx_builtin_include_directories = [
            "/usr/include/c++/7",
            "/usr/include/x86_64-linux-gnu/c++/7",
            "/usr/include/c++/7/backward",
            "/usr/lib/gcc/x86_64-linux-gnu/7/include",
            "/usr/local/include",
            "/usr/lib/gcc/x86_64-linux-gnu/7/include-fixed",
            "/usr/include/x86_64-linux-gnu",
            "/usr/include",
        ],
    )

cc_toolchain_config = rule(
    implementation = _impl,
    attrs = {},
    provides = [CcToolchainConfigInfo],
)