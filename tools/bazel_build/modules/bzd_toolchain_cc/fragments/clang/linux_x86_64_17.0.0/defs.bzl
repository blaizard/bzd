"""Metadata for Linux toolchains."""

load("@bzd_toolchain_cc//:fragments/clang/linux/defs.bzl", "linux")
load("@bzd_toolchain_cc//cc:toolchain.bzl", "get_location", "toolchain_maker", "toolchain_merge")

def linux_x86_64(module_ctx, name):
    """Metadata for Linux toolchains.

    Args:
        module_ctx: The module context.
        name: The name of the toolchain.

    Returns:
        The toolchain definition.
    """

    repository_path = get_location(module_ctx, name)
    toolchain_definition = {
        "build_files": [
            "@bzd_toolchain_cc//:fragments/clang/linux_x86_64_17.0.0/linux_x86_64.BUILD",
        ],
        "builtin_include_directories": [
            "/usr/include/x86_64-linux-gnu",
            "/usr/include",
        ],
        "compile_flags": [

            # Use C++20
            "-std=c++20",

            # Standard includes, this is needed to avoid indefined include complain from Bazel.
            "-nostdinc",
            "--no-standard-includes",
        ],
        "link_flags": [
            # Use the lld llvm linker.
            "-fuse-ld=lld",
            "-Wl,--disable-new-dtags",
            "-rdynamic",
            "-stdlib=libc++",
            "-lm",
            "-lc++",
            "-lc++abi",
            "-Wl,-z,relro,-z,now",
            "-no-canonical-prefixes",

            # Stamp the binary with a unique identifier
            "-Wl,--build-id=md5",
            "-Wl,--hash-style=gnu",
        ],
        "linker_dirs": [
            "{}/lib".format(repository_path),
        ],
        "sha256": "1a3a7a8df2ca2dd84358b8202c256ba93789871bd2329ba426337cdfdd9de4ef",
        "strip_prefix": "linux_x86_64_17.0.0",
        "system_directories": [
            "{}/include/c++/v1".format(repository_path),
            "{}/lib/clang/17/include".format(repository_path),
            "{}/include/x86_64-unknown-linux-gnu/c++/v1".format(repository_path),
            "/usr/include/x86_64-linux-gnu",
            "/usr/include",
        ],
        "url": "http://data.blaizard.com/file/bzd/toolchains/cc/clang/linux_x86_64/linux_x86_64_17.0.0.tar.xz",
    }

    toolchain_definition = toolchain_merge(toolchain_definition, linux(module_ctx, name))

    toolchain_maker(
        name = name,
        implementation = "linux_clang",
        definition = toolchain_definition,
    )

    return toolchain_definition

linux_x86_64_17_0_0 = {
    "linux-x86_64": linux_x86_64,
}
