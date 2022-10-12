load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("//tools/bazel_build/toolchains/cc:defs.bzl", "toolchain_maker")

def _load_linux_x86_64_clang_14_0_6(name):
    # Load dependencies
    package_name = "linux_x86_64_clang_14_0_6"
    http_archive(
        name = package_name,
        build_file = "//toolchains/cc/linux_x86_64_clang:{}.BUILD".format(package_name),
        urls = [
            "http://data.blaizard.com/file/bzd/toolchains/cc/clang/linux_x86_64/linux_x86_64_14.0.6.tar.xz",
        ],
        strip_prefix = "linux_x86_64_14.0.6",
        sha256 = "3d06ea0b7dcfec1851900e0c1b4f7dedc459a159be806ba0cb78347934d8b072",
    )

    toolchain_definition = {
        "default": True,
        "cpu": "linux_x86_64",
        "compiler": "clang",
        "exec_compatible_with": [
            "@platforms//os:linux",
            "@platforms//cpu:x86_64",
        ],
        "target_compatible_with": [
            "@platforms//os:linux",
            "@platforms//cpu:x86_64",
        ],
        "builtin_include_directories": [
            "/usr/include/x86_64-linux-gnu",
            "/usr/include",
        ],
        "system_directories": [
            "external/{}/include/c++/v1".format(package_name),
            "external/{}/lib/clang/14.0.6/include".format(package_name),
            "external/{}/include/x86_64-unknown-linux-gnu/c++/v1/".format(package_name),
            "/usr/include/x86_64-linux-gnu",
            "/usr/include",
        ],
        "linker_dirs": [
            "external/{}/lib".format(package_name),
        ],
        "compile_flags": [

            # Use C++20
            "-std=c++20",

            # Standard includes, this is needed to avoid indefined include complain from Bazel.
            "-nostdinc",
            "--no-standard-includes",
        ],
        "link_flags": [
            "-fuse-ld=lld",
            "-Wl,--disable-new-dtags",
            "-rdynamic",
            "-stdlib=libc++",
            "-lm",
            "-lc++",
            "-lc++abi",
            "-lc",
            "-lgcc_s",
            "-lgcc",
            "-Wl,-z,relro,-z,now",
            "-no-canonical-prefixes",

            # Stamp the binary with a unique identifier
            "-Wl,--build-id=md5",
            "-Wl,--hash-style=gnu",
        ],
        "dynamic_runtime_lib": [
            "@{}//:dynamic_libraries".format(package_name),
        ],
        "static_runtime_lib": [
            "@{}//:static_libraries".format(package_name),
        ],
        "filegroup_dependencies": [
            "@{}//:includes".format(package_name),
            "@{}//:bin".format(package_name),
        ],
        "bin_ar": "external/{}/bin/llvm-ar".format(package_name),
        "bin_as": "external/{}/bin/llvm-as".format(package_name),
        "bin_cc": "external/{}/bin/clang".format(package_name),
        "bin_cpp": "external/{}/bin/clang".format(package_name),
        "bin_cov": "external/{}/bin/llvm-cov".format(package_name),
        "bin_objdump": "external/{}/bin/llvm-objdump".format(package_name),
        "bin_ld": "external/{}/bin/clang++".format(package_name),
        "bin_strip": "external/{}/bin/llvm-strip".format(package_name),
        "app_executors": {
            "@//toolchains/cc/linux_x86_64_clang:executor_host": "default,host",
        },
    }

    toolchain_maker(
        name = name,
        implementation = "linux_clang",
        definition = toolchain_definition,
    )

def load_linux_x86_64_clang():
    _load_linux_x86_64_clang_14_0_6(name = "linux_x86_64_clang")
