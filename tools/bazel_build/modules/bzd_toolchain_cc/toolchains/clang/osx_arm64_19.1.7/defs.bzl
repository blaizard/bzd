"""Metadata for Linux toolchains."""

load("//cc:toolchain.bzl", "toolchain_maker", "toolchain_merge")
load("//toolchains/clang:unix/defs.bzl", "unix")

def osx_arm64(module_ctx, name, use_fragment):
    """Metadata for OSX toolchains.

    Args:
        module_ctx: The context of the module.
        name: The name of the toolchain.
        use_fragment: Factory to make use of a fragment.

    Returns:
        The toolchain definition.
    """

    _unix_name, unix_toolchain_definition = use_fragment(unix)

    result = module_ctx.execute(["xcrun", "--show-sdk-path"])
    sdk_root = result.stdout.strip() if result.return_code == 0 else "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk"

    toolchain_definition = {
        "build_files": [
            "@bzd_toolchain_cc//toolchains/clang:osx_arm64_19.1.7/osx_arm64.BUILD",
        ],
        "builtin_include_directories": [
            "{}/System/Library/Frameworks/Kernel.framework/Headers".format(sdk_root),
            "{}/usr/include".format(sdk_root),
        ],
        "compile_flags": [
            "-isysroot",
            sdk_root,
            "-nostdinc",
            "--no-standard-includes",
        ],
        "cxx_flags": [
            "-std=c++20",
        ],
        "host_system_directories": [
            "=/System/Library/Frameworks",
            "=/usr/include",
        ],
        "link_flags": [
            "-Wl,-syslibroot,{}".format(sdk_root),
            "-lc++",
            "-lc++abi",
            "-no-canonical-prefixes",
        ],
        "linker_dirs": [
            "@{}//lib".format(name),
        ],
        "package_name": "osx_arm64_19.1.7",
        "sha256": "d93bf12952d89fe4ec7501c40475718b722407da6a8d651f05c995863468e570",
        "strip_prefix": "LLVM-19.1.7-macOS-ARM64",
        "system_directories": [
            "@{}//include/c++/v1".format(name),
            "@{}//lib/clang/19/include".format(name),
        ],
        "urls": [
            "https://github.com/llvm/llvm-project/releases/download/llvmorg-19.1.7/LLVM-19.1.7-macOS-ARM64.tar.xz",
        ],
    }

    toolchain_definition = toolchain_merge(toolchain_definition, unix_toolchain_definition)

    toolchain_maker(
        name = name,
        implementation = "osx_clang",
        definition = toolchain_definition,
    )

    return toolchain_definition

osx_arm64_19_1_7 = {
    "osx-arm64": osx_arm64,
}
