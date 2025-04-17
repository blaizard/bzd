"""OCI image rule."""

load("@rules_oci//oci:defs.bzl", "oci_image", "oci_image_rule")

def _bzd_oci_image_impl(name, visibility, base, cmd, entrypoint, workdir, env, tars, **kwargs):
    oci_image(
        name = name,
        base = base,
        cmd = cmd,
        entrypoint = entrypoint,
        workdir = workdir,
        env = env,
        tars = tars,
        visibility = visibility,
        **kwargs
    )

bzd_oci_image = macro(
    doc = "Build a container image.",
    implementation = _bzd_oci_image_impl,
    inherit_attrs = oci_image_rule,
    attrs = {
        "base": attr.label(
            mandatory = True,
            doc = "Base image for the container.",
            configurable = False,
        ),
        "cmd": attr.string_list(
            doc = "Commands to be passed to the container.",
            configurable = False,
        ),
        "entrypoint": attr.string_list(
            doc = "Commands to be passed to the container.",
            configurable = False,
        ),
        "env": attr.string_dict(
            doc = "Environment variables.",
            configurable = False,
        ),
        "tars": attr.label_list(
            doc = "tarball representing the OCI layers.",
            allow_files = [".tar"],
        ),
        "workdir": attr.string(
            doc = "Working directory.",
            configurable = False,
        ),
    },
)
