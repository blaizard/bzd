"""Extension to access docker images."""

load("@rules_oci//oci:pull.bzl", "oci_pull")

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
