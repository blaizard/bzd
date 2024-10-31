"""Access docker images configuration."""

load("//:private/images.bzl", "IMAGES")

def _bzd_oci_images_config_impl(ctx):
    config_json = ctx.actions.declare_file(ctx.attr.out.name)
    ctx.actions.write(
        output = config_json,
        content = json.encode(IMAGES),
    )
    return [
        DefaultInfo(files = depset([config_json])),
    ]

bzd_oci_images_config = rule(
    doc = """Expose the docker images configuration as a json file.""",
    implementation = _bzd_oci_images_config_impl,
    attrs = {
        "out": attr.output(
            doc = "The name of the file created by the rule.",
        ),
    },
)
