load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

_repositories = {
    "clang_15.0.6": {
        "build_files": "@bzd_toolchain_cc//:toolchains/clang/linux_clang.BUILD",
        "urls": [
            "http://data.blaizard.com/file/bzd/toolchains/cc/clang/linux_x86_64/linux_x86_64_15.0.6.tar.xz",
        ],
        "strip_prefix": "linux_x86_64_15.0.6",
        "sha256": "53e7a89dffc0297a3ca208b49f9448b507d7298b01d53a36e03db0d2b4f4b896",
    }
}

def _toolchain_cc_impl(module_ctx):
    for mod in module_ctx.modules:
        for toolchain in mod.tags.toolchain:
            repositories = _repositories[toolchain.version]
            for arch, repository in repositories.items():

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
