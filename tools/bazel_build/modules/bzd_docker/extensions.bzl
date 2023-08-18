"""Extension to access docker images."""

load("@bzd_docker//:images.bzl", "IMAGES")
load("@rules_oci//oci:pull.bzl", "oci_pull")

def _images_repository_impl(repository_ctx):
    build_content = ""
    for name in IMAGES.keys():
        build_content += """
alias(
    name = "{name}",
    actual = "@{name}//:{name}",
    visibility = ["//visibility:public"],
)
""".format(
            name = name,
        )

    repository_ctx.file(
        "BUILD",
        executable = False,
        content = build_content,
    )

images_repository = repository_rule(
    implementation = _images_repository_impl,
    attrs = {},
)

def _images_impl(module_ctx):
    # Assign all image repos.
    for name, data in IMAGES.items():
        oci_pull(
            name = name,
            image = data.image,
            digest = data.digest,
        )

    for mod in module_ctx.modules:
        for data in mod.tags.install:
            images_repository(
                name = data.name,
            )

images = module_extension(
    implementation = _images_impl,
    tag_classes = {"install": tag_class(attrs = {"name": attr.string(
        mandatory = True,
        doc = "Name of the repository to hold the images.",
    )})},
)
