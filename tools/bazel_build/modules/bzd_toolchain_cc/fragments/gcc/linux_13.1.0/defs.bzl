"""Metadata for Linux toolchains."""

load("//:fragments/gcc/linux/defs.bzl", "linux")
load("//cc:toolchain.bzl", "toolchain_maker", "toolchain_merge")

def linux_x86_64(module_ctx, name):
    """Metadata for Linux toolchains.

    Args:
        module_ctx: The module context.
        name: The name of the toolchain.

    Returns:
        The toolchain definition.
    """

    toolchain_definition = {
        "build_files": [
            "@bzd_toolchain_cc//:fragments/gcc/linux_13.1.0/linux_x86_64.BUILD",
        ],
        "builtin_include_directories": [
            "/usr/include/x86_64-linux-gnu",
            "/usr/include",
        ],
        "compile_flags": [
            # There is a bug with coroutines
            "-Wno-unused-value",
        ],
        "cxx_flags": [
            "-std=c++20",
            "-fcoroutines",
        ],
        "host_system_directories": [
            "/usr/include/x86_64-linux-gnu",
            "/usr/include",
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
        "linker_dirs": [
            "@{}//lib/gcc/x86_64-pc-linux-gnu/13.1.0".format(name),
            "@{}//lib64".format(name),
        ],
        "package_name": "linux_x86_64_13.1.0",
        "sha256": "07f2855d072cb7de1ff1d81612d1d41cf6e548c20f4c16a09d268086e5690166",
        "strip_prefix": "linux_x86_64_13.1.0",
        "system_directories": [
            "@{}//include/c++/13.1.0/x86_64-pc-linux-gnu".format(name),
            "@{}//include/c++/13.1.0".format(name),
            "@{}//lib/gcc/x86_64-pc-linux-gnu/13.1.0/include".format(name),
            "@{}//lib/gcc/x86_64-pc-linux-gnu/13.1.0/include-fixed".format(name),
        ],
        "urls": [
            "https://datalocal.blaizard.com/file/bzd/toolchains/cc/gcc/linux_x86_64/linux_x86_64_13.1.0.tar.xz",
            "https://data.blaizard.com/file/bzd/toolchains/cc/gcc/linux_x86_64/linux_x86_64_13.1.0.tar.xz",
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
    "linux-x86_64": linux_x86_64,
}
