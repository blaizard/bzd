"""Registry for docker images used in this repository."""

load("@bzd_docker//:images.bzl", "IMAGES")
load("@rules_oci//oci:pull.bzl", "oci_pull")

def _bzd_docker_images_config_impl(ctx):
    config_json = ctx.actions.declare_file(ctx.attr.out.name)
    ctx.actions.write(
        output = config_json,
        content = json.encode(IMAGES),
    )
    return [
        DefaultInfo(files = depset([config_json])),
    ]

bzd_docker_images_config = rule(
    doc = """Expose the docker images configuration as a json file.""",
    implementation = _bzd_docker_images_config_impl,
    attrs = {
        "out": attr.output(
            doc = "The name of the file created by the rule.",
        ),
    },
)

def bzd_docker_pull(name, image, digest):
    """Pull a docker image locally.

    Args:
        name: The name of the bazel target.
        image: The image name.
        digest: The digest of the image.
    """

    oci_pull(
        name = name,
        image = image,
        digest = digest,
    )
