"""OCI application rule."""

load("@bzd_bdl//:defs.bzl", "bdl_application_factory")

def _bzd_oci_application_impl(ctx, bdl, namespace):
    oci_directory = ctx.attr.target[DefaultInfo].files.to_list()[0]
    ctx.actions.write(
        output = bdl,
        content = """
    namespace {namespace};
    image = String("{oci_directory}");
    """.format(
            namespace = namespace,
            oci_directory = oci_directory.short_path,
        ),
    )

    return [DefaultInfo(runfiles = ctx.runfiles(
        files = ctx.attr.target[DefaultInfo].files.to_list(),
    ))]

bzd_oci_application = bdl_application_factory(_bzd_oci_application_impl)
