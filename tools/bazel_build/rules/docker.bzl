load("@io_bazel_rules_docker//container:container.bzl", "container_pull")

def bzd_docker_pull(**kwargs):
    """Pull a docker image locally."""

    container_pull(timeout = 3600, **kwargs)

def _bzd_docker_load_impl(ctx):
    ctx.actions.write(
        output = ctx.outputs.executable,
        content = """#!/bin/bash
        docker pull {registry}/{repository}@{digest}
        """.format(
            digest = ctx.attr.digest,
            registry = ctx.attr.registry,
            repository = ctx.attr.repository,
        ),
        is_executable = True,
    )

    return [DefaultInfo(executable = ctx.outputs.executable)]

bzd_docker_load = rule(
    doc = "Load a docker image.",
    implementation = _bzd_docker_load_impl,
    attrs = {
        "registry": attr.string(
            mandatory = True,
            doc = "The registry from which we are pulling.",
        ),
        "repository": attr.string(
            mandatory = True,
            doc = "The name of the image.",
        ),
        "digest": attr.string(
            doc = "The digest of the image to pull.",
        ),
    },
    executable = True,
)
