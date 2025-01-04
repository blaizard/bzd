"""Metadata for Linux toolchains."""

load("//:fragments/clang/linux/defs.bzl", "linux")
load("//cc:toolchain.bzl", "toolchain_maker", "toolchain_merge")

def linux_arm64(module_ctx, name):
    """Metadata for Linux toolchains.

    Args:
        module_ctx: The module context.
        name: The name of the toolchain.

    Returns:
        The toolchain definition.
    """

    toolchain_definition = {
        "build_files": [
            "@bzd_toolchain_cc//:fragments/clang/linux_arm64_17.0.0/linux_arm64.BUILD",
        ],
        "builtin_include_directories": [
            "/usr/include/aarch64-linux-gnu",
            "/usr/include",
        ],
        "compile_flags": [
            # Standard includes, this is needed to avoid indefined include complain from Bazel.
            "-nostdinc",
            "--no-standard-includes",
        ],
        "cxx_flags": [
            "-std=c++20",
        ],
        "host_system_directories": [
            "/usr/include/aarch64-linux-gnu",
            "/usr/include",
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
            "@{}//lib".format(name),
        ],
        "package_name": "linux_arm64_17.0.0",
        "sha256": "d49b50d8d5b9cdec00d15f705be5627488f56c4210437d4d07870bed8c4ab2f2",
        "strip_prefix": "linux_arm64_17.0.0",
        "system_directories": [
            "@{}//include/c++/v1".format(name),
            "@{}//lib/clang/17/include".format(name),
            "@{}//include/aarch64-unknown-linux-gnu/c++/v1".format(name),
        ],
        "urls": [
            "https://datalocal.blaizard.com/file/bzd/toolchains/cc/clang/linux_arm64/linux_arm64_17.0.0.tar.xz",
            "https://data.blaizard.com/file/bzd/toolchains/cc/clang/linux_arm64/linux_arm64_17.0.0.tar.xz",
        ],
    }

    toolchain_definition = toolchain_merge(toolchain_definition, linux(module_ctx, name))

    toolchain_maker(
        name = name,
        implementation = "linux_clang",
        definition = toolchain_definition,
    )

    return toolchain_definition

linux_arm64_17_0_0 = {
    "linux-arm64": linux_arm64,
}
