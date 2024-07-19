"""OCI application rule."""

load("@bzd_bdl//:defs.bzl", "bdl_application_factory")
load("@bzd_lib//config:defs.bzl", "bzd_config_apply")

def _bzd_oci_application_impl(ctx, target, bdl, namespace):
    oci_directory = target[DefaultInfo].files.to_list()[0]
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
        files = target[DefaultInfo].files.to_list(),
    ))]

_bzd_oci_application = bdl_application_factory(_bzd_oci_application_impl)

def bzd_oci_application(name, target, configs = None, **kwargs):
    if configs:
        bzd_config_apply(
            name = "{}.config_apply".format(name),
            target = target,
            configs = configs,
        )

        _bzd_oci_application(
            name = name,
            target = "{}.config_apply".format(name),
            **kwargs
        )

    else:
        _bzd_oci_application(
            name = name,
            target = target,
            **kwargs
        )
