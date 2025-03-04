"""Rules to upload a target into artifacts."""

load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")
load("@bzd_lib//config:defs.bzl", "ConfigInfo", "bzd_config")

def _platform_transition_impl(_settings, attr):
    if attr.platform:
        return {"//command_line_option:platforms": str(attr.platform)}
    return None

platform_transition = transition(
    implementation = _platform_transition_impl,
    inputs = [],
    outputs = ["//command_line_option:platforms"],
)

def _bzd_artifacts_upload_impl(ctx):
    kwargs = {
        "config": ctx.attr.config[ConfigInfo].json.short_path,
        "version": ctx.attr.config_version,
    }
    command = ["{{binary}}", "--config", "\"{config}\"", "--config-version", "{version}"]
    data = [ctx.attr.config[ConfigInfo].json]

    if not ctx.attr.target:
        if ctx.attr.url:
            fail("The 'url' cannot be set if the 'target' is not set.")
    else:
        kwargs["artifact"] = ctx.file.target.short_path
        command.append("\"{artifact}\"")
        data.append(ctx.file.target)
        if ctx.attr.url:
            kwargs["url"] = ctx.attr.url
            command.append("\"{url}\"")
    command.append("\"$@\"")

    return [sh_binary_wrapper_impl(
        ctx = ctx,
        binary = ctx.attr._upload,
        output = ctx.outputs.executable,
        command = " ".join(command).format(**kwargs),
        data = data,
    )]

_bzd_artifacts_upload = rule(
    implementation = _bzd_artifacts_upload_impl,
    attrs = {
        "config": attr.label(
            doc = "The configuration to extract the version.",
            providers = [ConfigInfo],
            mandatory = True,
        ),
        "config_version": attr.string(
            doc = "The key in the config corresponding to the version, it will be used as filename.",
            mandatory = True,
        ),
        "platform": attr.label(
            doc = "The platform to be used.",
        ),
        "target": attr.label(
            doc = "The target to be released.",
            allow_single_file = True,
            cfg = platform_transition,
        ),
        "url": attr.string(
            doc = "The url to publish to.",
        ),
        "_upload": attr.label(
            default = Label("//apps/artifacts/api/bazel/upload"),
            cfg = "exec",
            executable = True,
        ),
    },
    executable = True,
)

def _bzd_artifacts_upload_macro_impl(name, visibility, **kwargs):
    bzd_config(
        name = "{}.config".format(name),
        # Note, STABLE_VERSION name was chosen to have it part of of the stable-info.txt file
        # which is the one that trigger an action rebuild, the volatile-info.txt does not.
        # See: https://bazel.build/docs/user-manual#workspace-status
        include_workspace_status = ["STABLE_VERSION"],
    )

    _bzd_artifacts_upload(
        name = name,
        visibility = visibility,
        config = "{}.config".format(name),
        config_version = "STABLE_VERSION",
        **kwargs
    )

bzd_artifacts_upload = macro(
    doc = "Upload a file to the artifact server.",
    implementation = _bzd_artifacts_upload_macro_impl,
    inherit_attrs = _bzd_artifacts_upload,
    attrs = {
        "config": None,
        "config_version": None,
    },
)
