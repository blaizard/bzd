"""Metadata for Linux toolchains."""

load("//:fragments/gcc/linux/defs.bzl", "linux")
load("//cc:toolchain.bzl", "toolchain_maker", "toolchain_merge")

def linux_x86_64(name, use_fragment):
    """Metadata for Linux toolchains.

    Args:
        name: The name of the toolchain.
        use_fragment: Factory to make use of a fragment.

    Returns:
        The toolchain definition.
    """

    _linux_name, linux_toolchain_definition = use_fragment(linux)

    toolchain_definition = {
        "build_files": [
            "@bzd_toolchain_cc//:fragments/gcc/linux_14.2.0/linux_x86_64.BUILD",
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
            "@{}//lib/gcc/x86_64-pc-linux-gnu/14.2.0".format(name),
            "@{}//lib64".format(name),
        ],
        "package_name": "linux_x86_64_14.2.0",
        "sha256": "b5e10c57b12a5ef1150f2f3ef4fd78f7865dcc899b76b24d8f3958fb2a74d06f",
        "strip_prefix": "linux_x86_64_14.2.0",
        "system_directories": [
            "@{}//include/c++/14.2.0/x86_64-pc-linux-gnu".format(name),
            "@{}//include/c++/14.2.0".format(name),
            "@{}//lib/gcc/x86_64-pc-linux-gnu/14.2.0/include".format(name),
            "@{}//lib/gcc/x86_64-pc-linux-gnu/14.2.0/include-fixed".format(name),
        ],
        "urls": [
            "https://datalocal.blaizard.com/file/bzd/toolchains/cc/gcc/linux_x86_64/linux_x86_64_14.2.0.tar.xz",
            "https://data.blaizard.com/file/bzd/toolchains/cc/gcc/linux_x86_64/linux_x86_64_14.2.0.tar.xz",
        ],
    }

    toolchain_definition = toolchain_merge(toolchain_definition, linux_toolchain_definition)

    toolchain_maker(
        name = name,
        implementation = "linux_gcc",
        definition = toolchain_definition,
    )

    return toolchain_definition

linux_x86_64_14_2_0 = {
    "linux-x86_64": linux_x86_64,
}
