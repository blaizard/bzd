"""Extension to access docker images."""

load("@bzd_lib//:defs.bzl", "bzd_http_archive", "bzd_repository_multiplatform_maker")
load("@rules_oci//oci:pull.bzl", "oci_pull")

def _load_podman(_module_ctx):
    bzd_repository_multiplatform_maker(
        name = "podman",
        repositories = [
            {
                "build_file": "//toolchain:podman.BUILD.bazel",
                "compatible_with": [
                    Label("@bzd_platforms//al_isa:linux-x86_64"),
                ],
                "http": [
                    bzd_http_archive(
                        sha256 = "a58765fe8be6ab3fb79f892f1a027b4ce4a7e8eb589df1ef960c167cbde08d69",
                        strip_prefix = "podman-linux-amd64",
                        url = "https://data.blaizard.com/file/bzd/toolchains/podman/5.8.4/podman-linux-amd64.tar.gz",
                    ),
                ],
            },
            {
                "build_file": "//toolchain:podman.BUILD.bazel",
                "compatible_with": [
                    Label("@bzd_platforms//al_isa:linux-arm64"),
                ],
                "http": [
                    bzd_http_archive(
                        sha256 = "a2f6b73cc0f7018e2e8518338a4ec27db70148e1af86e16719235605aefd1df3",
                        strip_prefix = "podman-linux-arm64",
                        url = "https://data.blaizard.com/file/bzd/toolchains/podman/5.8.4/podman-linux-arm64.tar.gz",
                    ),
                ],
            },
        ],
        expose = {
            "podman": "podman",
        },
    )

def _load_dependencies_impl(module_ctx):
    _load_podman(module_ctx)

load_dependencies = module_extension(
    implementation = _load_dependencies_impl,
)

def _images_impl(module_ctx):
    for mod in module_ctx.modules:
        for data in mod.tags.pull:
            oci_pull(
                name = data.name,
                image = data.image,
                digest = data.digest,
                platforms = data.platforms,
            )

images = module_extension(
    implementation = _images_impl,
    tag_classes = {
        "pull": tag_class(attrs = {
            "digest": attr.string(
                mandatory = True,
                doc = "The digest of the image to be pulled.",
            ),
            "image": attr.string(
                mandatory = True,
                doc = "The name of the image to be pulled.",
            ),
            "name": attr.string(
                mandatory = True,
                doc = "The identifier of the image to be pulled.",
            ),
            "platforms": attr.string_list(
                doc = "The platforms associated with this image.",
            ),
        }),
    },
)
