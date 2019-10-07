load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("//tools/bazel:toolchain.bzl", "toolchain_maker")

def load_linux_x86_64_clang_9_0_0():

    # Load dependencies
    clang_package_name = "linux_x86_64_clang_9_0_0"
    http_archive(
        name = clang_package_name,
		build_file = "//tools/bazel:toolchains/{}.BUILD".format(clang_package_name),
        urls = [
            "http://releases.llvm.org/9.0.0/clang+llvm-9.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz",
        ],
        strip_prefix = "clang+llvm-9.0.0-x86_64-linux-gnu-ubuntu-18.04",
        sha256 = "a23b082b30c128c9831dbdd96edad26b43f56624d0ad0ea9edec506f5385038d"
    )

    toolchain_maker(
        name = "linux_x86_64_clang",

		cpu = "linux_x86_64",
        compiler = "clang",
        exec_compatible_with = [
            "@bazel_tools//platforms:linux",
            "@bazel_tools//platforms:x86_64",
        ],
        target_compatible_with = [
            "@bazel_tools//platforms:linux",
            "@bazel_tools//platforms:x86_64",
        ],

        builtin_include_directories = [
		    "/usr/include/x86_64-linux-gnu",
            "/usr/local/include",
            "/usr/include",
            "%package(@{}//)%/include/c++/v1".format(clang_package_name),
            "%package(@{}//)%/lib/clang/9.0.0/include".format(clang_package_name),
		],

        system_directories = [
		    "/usr/include/x86_64-linux-gnu",
            "/usr/local/include",
            "/usr/include",
            "external/{}/include/c++/v1".format(clang_package_name),
            "external/{}/lib/clang/9.0.0/include".format(clang_package_name),
		],

        linker_dirs = [
            'external/{}/lib'.format(clang_package_name),
        ],

        compile_flags = [

            "-std=c++14",

            # Do not link or re-order inclusion files
            "-nostdinc++",
            "-nostdinc",

            # Make the compilation deterministic
            "-U_FORTIFY_SOURCE",
            "-D_FORTIFY_SOURCE=1",
            "-fstack-protector",
            "-fPIE",
            "-no-canonical-prefixes",

            # Warnings
            "-Wall",
            "-Wno-missing-braces",
            "-Wno-builtin-macro-redefined",

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
        ],

		link_flags = [
            "-Wl,-as-needed",
            "-fuse-ld=gold",
            "-Wl,--disable-new-dtags",
            "-Wl,--gc-sections",
            "-rdynamic",
            "-lm",
		    "-lc++",
            "-Wl,-z,relro,-z,now",
            "-no-canonical-prefixes",

            # Stamp the binary with a unique identifier
            "-Wl,--build-id=md5",
            "-Wl,--hash-style=gnu"
        ],

        dynamic_runtime_libs = [
            "@{}//:dynamic_libraries".format(clang_package_name),
        ],

        static_runtime_libs = [
            "@{}//:static_libraries".format(clang_package_name),
        ],

		filegroup_dependencies = [
            "@{}//:includes".format(clang_package_name),
            "@{}//:bin".format(clang_package_name),
		],

        bin_ar = "external/{}/bin/llvm-ar".format(clang_package_name),
        bin_as = "external/{}/bin/llvm-as".format(clang_package_name),
        bin_cc =  "external/{}/bin/clang".format(clang_package_name),
        bin_cpp = "external/{}/bin/clang".format(clang_package_name),
        bin_gcov = "external/{}/bin/llvm-cov".format(clang_package_name),
        bin_objdump = "external/{}/bin/llvm-objdump".format(clang_package_name),
    )
