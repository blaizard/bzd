_repositories = {
    "clang_15.0.6": {
        # Target
        "linux_x86_64": {
            # Host
            "linux_x86_64": {
                "build_files": "@bzd_toolchain_cc//:toolchains/clang/linux_clang.BUILD",
                "urls": [
                    "http://data.blaizard.com/file/bzd/toolchains/cc/clang/linux_x86_64/linux_x86_64_15.0.6.tar.xz",
                ],
                "strip_prefix": "linux_x86_64_15.0.6",
                "sha256": "53e7a89dffc0297a3ca208b49f9448b507d7298b01d53a36e03db0d2b4f4b896",
            },
        },
    },
    "gcc_11.2.0": {
        "linux_x86_64": {
            "linux_x86_64": {
                "urls": [
                    "http://data.blaizard.com/file/bzd/toolchains/cc/gcc/linux_x86_64/linux_x86_64_11.2.0.tar.xz",
                ],
                "strip_prefix": "linux_x86_64_11.2.0",
                "sha256": "e7322906acb4671afb23e7e8f7cc68528495392a8e640f8e146b572e411897fa",
            },
        },
        "esp32_xtensa_lx6": {
            "linux_x86_64": {
                "urls": [
                    "http://data.blaizard.com/file/bzd/toolchains/cc/gcc/esp32_xtensa_lx6/xtensa-esp32-elf-gcc11_2_0-esp-2022r1-linux-amd64.tar.xz",
                ],
                "strip_prefix": "xtensa-esp32-elf",
                "sha256": "698d8407e18275d18feb7d1afdb68800b97904fbe39080422fb8609afa49df30",
            },
        },
    },
}

def _toolchain_cc_impl(module_ctx):
    for mod in module_ctx.modules:
        for toolchain in mod.tags.toolchain:
            repositories = _repositories[toolchain.version]
            for arch, repository in repositories.items():
                pass

toolchain_cc = module_extension(
    implementation = _toolchain_cc_impl,
    tag_classes = {
        "toolchain": tag_class(
            attrs = {
                "name": attr.string(mandatory = True),
                "version": attr.string(values = _repositories.keys(), mandatory = True),
            },
        ),
    },
)
