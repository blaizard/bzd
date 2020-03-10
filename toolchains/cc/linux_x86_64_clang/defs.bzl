load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("//tools/bazel.build/toolchains:defs.bzl", "toolchain_maker")
load("//toolchains/cc:defs.bzl", "COPTS_CLANG", "LINKOPTS_CLANG")

def _load_linux_x86_64_clang_9_0_0(name):
    # Load dependencies
    clang_package_name = "linux_x86_64_clang_9_0_0"
    http_archive(
        name = clang_package_name,
        build_file = "//toolchains/cc/linux_x86_64_clang:{}.BUILD".format(clang_package_name),
        urls = [
            "http://releases.llvm.org/9.0.0/clang+llvm-9.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz",
        ],
        strip_prefix = "clang+llvm-9.0.0-x86_64-linux-gnu-ubuntu-18.04",
        sha256 = "a23b082b30c128c9831dbdd96edad26b43f56624d0ad0ea9edec506f5385038d",
    )

    toolchain_definition = {
        "cpu": "linux_x86_64",
        "compiler": "clang",
        "platforms": [
            "@//tools/bazel.build/platforms:linux_x86_64",
        ],
        "exec_compatible_with": [
            "@//tools/bazel.build/platforms/al:linux",
            "@//tools/bazel.build/platforms/isa:x86_64",
        ],
        "target_compatible_with": [
            "@//tools/bazel.build/platforms/al:linux",
            "@//tools/bazel.build/platforms/isa:x86_64",
        ],
        "builtin_include_directories": [
            "/usr/include/x86_64-linux-gnu",
            "/usr/include",
            "%package(@{}//)%/include/c++/v1".format(clang_package_name),
            "%package(@{}//)%/lib/clang/9.0.0/include".format(clang_package_name),
        ],
        "system_directories": [
            "/usr/include/x86_64-linux-gnu",
            "/usr/include",
            "external/{}/include/c++/v1".format(clang_package_name),
            "external/{}/lib/clang/9.0.0/include".format(clang_package_name),
        ],
        "linker_dirs": [
            "external/{}/lib".format(clang_package_name),
        ],
        "compile_flags": [
            "-std=c++14",

            # Do not link or re-order inclusion files
            "-nostdinc++",
            #"-nostdinc",
            "--no-standard-includes",

            # Make the compilation deterministic
            "-fstack-protector",
            "-fPIE",
            "-no-canonical-prefixes",

            # Warnings
            "-Wall",
            "-Wno-missing-braces",
            "-Wno-builtin-macro-redefined",
            "-Wno-unused-command-line-argument",

            # Keep stack frames for debugging
            "-fno-omit-frame-pointer",

            # Optimization
            "-O3",

            # Removal of unused code and data at link time
            "-ffunction-sections",
            "-fdata-sections",

            # Use linkstamping instead of these
            "-D__DATE__=\"redacted\"",
            "-D__TIMESTAMP__=\"redacted\"",
            "-D__TIME__=\"redacted\"",
        ] + COPTS_CLANG,
        "link_flags": LINKOPTS_CLANG + [
            "-fuse-ld=lld",
            "-Wl,--disable-new-dtags",
            "-Wl,--gc-sections",
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
            "-Wl,-rpath=%{{absolute_external}}/{}/lib".format(clang_package_name),

            # Stamp the binary with a unique identifier
            "-Wl,--build-id=md5",
            "-Wl,--hash-style=gnu",
        ],
        "dynamic_runtime_libs": [
            "@{}//:dynamic_libraries".format(clang_package_name),
        ],
        "static_runtime_libs": [
            "@{}//:static_libraries".format(clang_package_name),
        ],
        "filegroup_dependencies": [
            "@{}//:includes".format(clang_package_name),
            "@{}//:bin".format(clang_package_name),
        ],
        "bin_ar": "external/{}/bin/llvm-ar".format(clang_package_name),
        "bin_as": "external/{}/bin/llvm-as".format(clang_package_name),
        "bin_cc": "external/{}/bin/clang".format(clang_package_name),
        "bin_cpp": "external/{}/bin/clang".format(clang_package_name),
        "bin_cov": "external/{}/bin/llvm-cov".format(clang_package_name),
        "bin_objdump": "external/{}/bin/llvm-objdump".format(clang_package_name),
        "bin_ld": "external/{}/bin/clang++".format(clang_package_name),
    }

    toolchain_maker(
        name = name,
        implementation = "linux",
        definition = toolchain_definition,
    )

def load_linux_x86_64_clang():
    _load_linux_x86_64_clang_9_0_0(name = "linux_x86_64_clang")
