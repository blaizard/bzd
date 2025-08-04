"""Simple docker binary."""

load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")

def _bzd_docker_binary_impl(ctx):
    args = []
    if ctx.attr.expose:
        args += ["--expose", str(ctx.attr.expose)]
    args.append(ctx.attr.image)

    return sh_binary_wrapper_impl(
        ctx = ctx,
        locations = {
            ctx.attr._docker: "docker",
        },
        output = ctx.outputs.executable,
        command = "{docker} " + " ".join(args),
    )

bzd_docker_binary = rule(
    doc = "Docker binary.",
    implementation = _bzd_docker_binary_impl,
    attrs = {
        "expose": attr.int(
            doc = "Port to expose.",
        ),
        "image": attr.string(
            mandatory = True,
            doc = "The name of the Docker image to use.",
        ),
        "_docker": attr.label(
            default = Label("//apps/external:docker"),
            executable = True,
            cfg = "target",
        ),
    },
    executable = True,
)
