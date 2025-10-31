"""Docker binary based on docker-compose."""

load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")
load("@bzd_lib//config:defs.bzl", "bzd_config_default")

def _bzd_docker_json_binary_impl(ctx):
    args = []
    if ctx.attr.expose:
        args += ["--expose", str(ctx.attr.expose)]
    args.append(ctx.label.name)

    return sh_binary_wrapper_impl(
        ctx = ctx,
        locations = {
            ctx.attr._docker: "docker",
            ctx.file.docker_compose: "docker_compose",
        },
        output = ctx.outputs.executable,
        command = "{docker} " + " ".join(args) + " {docker_compose} \"$@\"",
    )

_bzd_docker_json_binary = rule(
    doc = "Docker binary.",
    implementation = _bzd_docker_json_binary_impl,
    attrs = {
        "docker_compose": attr.label(
            mandatory = True,
            allow_single_file = True,
            doc = "The name of the Docker image to use.",
        ),
        "expose": attr.int(
            doc = "Port to expose.",
        ),
        "_docker": attr.label(
            default = Label("//apps/external:docker"),
            executable = True,
            cfg = "target",
        ),
    },
    executable = True,
)

def _bzd_docker_binary_impl(name, visibility, docker_compose, **kwargs):
    bzd_config_default(
        name = "{}.config".format(name),
        srcs = [
            docker_compose,
        ],
    )

    _bzd_docker_json_binary(
        name = name,
        visibility = visibility,
        docker_compose = "{}.config.json".format(name),
        **kwargs
    )

bzd_docker_binary = macro(
    inherit_attrs = _bzd_docker_json_binary,
    attrs = {
        "docker_compose": attr.label(
            allow_single_file = ["yaml"],
            mandatory = True,
            doc = "The docker-compose.yaml file associated with this instance.",
            configurable = False,
        ),
    },
    implementation = _bzd_docker_binary_impl,
)
