"""Rules to upload a target into artifacts."""

load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")
load("@bzd_lib//config:defs.bzl", "bzd_config")

def _bzd_artifacts_release_impl(ctx):
    return [sh_binary_wrapper_impl(
        ctx = ctx,
        binary = ctx.attr._release,
        output = ctx.outputs.executable,
        command = "{{binary}} --config \"{config}\" --config-version {version} \"{artifact}\" \"{url}\"".format(
            config = ctx.file.config.short_path,
            version = ctx.attr.config_version,
            artifact = ctx.file.target.short_path,
            url = ctx.attr.url,
        ),
        data = [ctx.file.target, ctx.file.config],
    )]

_bzd_artifacts_release = rule(
    implementation = _bzd_artifacts_release_impl,
    attrs = {
        "config": attr.label(
            doc = "The configuration to extract the version.",
            allow_single_file = True,
            mandatory = True,
        ),
        "config_version": attr.string(
            doc = "The key in the config corresponding to the version, it will be used as filename.",
            mandatory = True,
        ),
        "target": attr.label(
            doc = "The target to be released.",
            allow_single_file = True,
            mandatory = True,
            cfg = "target",
        ),
        "url": attr.string(
            doc = "The url to publish to.",
            mandatory = True,
        ),
        "_release": attr.label(
            default = Label("//apps/artifacts/bazel:release"),
            cfg = "exec",
            executable = True,
        ),
    },
    executable = True,
)

def bzd_artifacts_release(name, **kwargs):
    bzd_config(
        name = "{}.config".format(name),
        include_workspace_status = ["STABLE_VERSION"],
        visibility = ["//visibility:private"],
    )

    _bzd_artifacts_release(
        name = name,
        config = "{}.config".format(name),
        config_version = "STABLE_VERSION",
        **kwargs
    )
