load("@bzd_toolchain_cc//cc:defs.bzl", "get_location", "toolchain_maker", "toolchain_merge")
load("@bzd_toolchain_cc//:fragments/clang/linux/defs.bzl", "linux")

def linux_x86_64(module_ctx, name):
    repository_path = get_location(module_ctx, name)
    toolchain_definition = {
        "url": "http://data.blaizard.com/file/bzd/toolchains/cc/clang/linux_x86_64/linux_x86_64_17.0.0.tar.xz",
        "strip_prefix": "linux_x86_64_17.0.0",
        "sha256": "1a3a7a8df2ca2dd84358b8202c256ba93789871bd2329ba426337cdfdd9de4ef",
        "build_files": [
            "@bzd_toolchain_cc//:fragments/clang/linux_x86_64_17.0.0/linux_x86_64.BUILD",
        ],
        "builtin_include_directories": [
            "/usr/include/x86_64-linux-gnu",
            "/usr/include",
        ],
        "system_directories": [
            "{}/include/c++/v1".format(repository_path),
            "{}/lib/clang/17/include".format(repository_path),
            "{}/include/x86_64-unknown-linux-gnu/c++/v1".format(repository_path),
            "/usr/include/x86_64-linux-gnu",
            "/usr/include",
        ],
        "linker_dirs": [
            "{}/lib".format(repository_path),
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
