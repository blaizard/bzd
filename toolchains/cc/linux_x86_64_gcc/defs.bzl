load("//tools/bazel_build/toolchains/cc:defs.bzl", "toolchain_maker")

def _load_linux_x86_64_gcc_11_2_0(name):
    # Load dependencies
    package_name = "linux_x86_64_gcc_11_2_0"

    toolchain_definition = {
        "cpu": "linux_x86_64",
        "compiler": "gcc",
        "docker_image": "docker://docker.io/blaizard/linux_x86_64_gcc:11.2.0",
        "exec_compatible_with": [
            "@platforms//os:linux",
            "@platforms//cpu:x86_64",
        ],
        "target_compatible_with": [
            "@platforms//os:linux",
            "@platforms//cpu:x86_64",
        ],
        "builtin_include_directories": [
            "/usr/lib/gcc/x86_64-linux-gnu/11/include",
            "/usr/include/c++/11",
            "/usr/include/x86_64-linux-gnu/c++/11",
            "/usr/lib/gcc/x86_64-linux-gnu/11/include",
            "/usr/lib/gcc/x86_64-linux-gnu/11/include-fixed",
            "/usr/include/x86_64-linux-gnu",
            "/usr/include",
        ],
        "system_directories": [
            "/usr/include/c++/11",
            "/usr/include/x86_64-linux-gnu/c++/11",
            "/usr/lib/gcc/x86_64-linux-gnu/11/include",
            "/usr/lib/gcc/x86_64-linux-gnu/11/include-fixed",
            "/usr/include/x86_64-linux-gnu",
            "/usr/include",
        ],
        "linker_dirs": [
            "/usr/lib/gcc/x86_64-linux-gnu/11",
            "/usr/lib/x86_64-linux-gnu",
        ],
        "compile_flags": [

            # Use C++20.
            "-std=c++20",
            "-fcoroutines",

            # There is a bug with coroutines
            "-Wno-unused-value",
        ],
        "link_flags": [
            "-Wl,--disable-new-dtags",
            "-Wl,--gc-sections",
            "-rdynamic",
            "-Wl,-z,relro,-z,now",
            "-lstdc++",
        ],
        "dynamic_runtime_libs": [
            #"@{}//:dynamic_libraries".format(package_name),
        ],
        "static_runtime_libs": [
            #"@{}//:static_libraries".format(package_name),
        ],
        "filegroup_dependencies": [
            #"@{}//:includes".format(package_name),
            #"@{}//:bin".format(package_name),
        ],
        "bin_ar": "/usr/bin/x86_64-linux-gnu-ar",
        "bin_as": "/usr/bin/x86_64-linux-gnu-as",
        "bin_cc": "/usr/bin/x86_64-linux-gnu-gcc-11",
        "bin_cpp": "/usr/bin/x86_64-linux-gnu-cpp-11",
        "bin_cov": "/usr/bin/x86_64-linux-gnu-gcov-11",
        "bin_objdump": "/usr/bin/x86_64-linux-gnu-objdump",
        "bin_ld": "/usr/bin/x86_64-linux-gnu-ld",
        "bin_strip": "/usr/bin/x86_64-linux-gnu-strip",
        "app_executors": {
            "@//tools/bazel_build/toolchains/cc:executor_host": "default,host",
        },
    }

    toolchain_maker(
        name = name,
        implementation = "linux_gcc",
        definition = toolchain_definition,
    )

def load_linux_x86_64_gcc():
    _load_linux_x86_64_gcc_11_2_0(name = "linux_x86_64_gcc")
