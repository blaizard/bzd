load("@bzd_toolchain_cc//cc:defs.bzl", "toolchain_maker", "toolchain_merge", "get_location")
load("@bzd_toolchain_cc//:fragments/gcc/linux/defs.bzl", "linux")

def linux_x86_64(module_ctx, name):

    repository_path = get_location(module_ctx, name)
    toolchain_definition = {
        "url": "http://data.blaizard.com/file/bzd/toolchains/cc/gcc/linux_x86_64/linux_x86_64_13.1.0.tar.xz",
        "strip_prefix": "linux_x86_64_13.1.0",
        "sha256": "07f2855d072cb7de1ff1d81612d1d41cf6e548c20f4c16a09d268086e5690166",
        "build_files": [
            "@bzd_toolchain_cc//:fragments/gcc/linux_13.1.0/linux_x86_64.BUILD"
        ],
        "builtin_include_directories": [
            "/usr/include/x86_64-linux-gnu",
            "/usr/include",
        ],
        "system_directories": [
            "{}/include/c++/13.1.0/x86_64-pc-linux-gnu".format(repository_path),
            "{}/include/c++/13.1.0".format(repository_path),
            "{}/lib/gcc/x86_64-pc-linux-gnu/13.1.0/include".format(repository_path),
            "{}/lib/gcc/x86_64-pc-linux-gnu/13.1.0/include-fixed".format(repository_path),
            "/usr/include/x86_64-linux-gnu",
            "/usr/include",
        ],
        "linker_dirs": [
            "{}/lib/gcc/x86_64-pc-linux-gnu/13.1.0".format(repository_path),
            "{}/lib64".format(repository_path),
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
            "-rdynamic",
            "-Wl,-z,relro,-z,now",
            # Link statically with the provided libstdc++.
            # This is to avoid relying on a different libstdc++ at runtime, which improves reproductibility.
            "-static-libstdc++",
            "-lm",
        ],
    }

    toolchain_definition = toolchain_merge(toolchain_definition, linux(module_ctx, name))

    toolchain_maker(
        name = name,
        implementation = "linux_gcc",
        definition = toolchain_definition,
    )

    return toolchain_definition

linux_x86_64_13_1_0 = {
    "linux-x86_64": linux_x86_64
}
