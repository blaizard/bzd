"""Docker binary based on docker-compose."""

load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")
load("@bzd_lib//config:defs.bzl", "bzd_config_default")

def _bzd_oci_json_binary_impl(ctx):
    oci_runner = ctx.toolchains["@bzd_rules_oci//toolchain:toolchain_type"].oci_runner

    return sh_binary_wrapper_impl(
        ctx = ctx,
        locations = {
            ctx.attr._runner: "runner",
            oci_runner: "oci_runner",
            ctx.file.docker_compose: "docker_compose",
        },
        output = ctx.outputs.executable,
        data = ctx.files.data,
        command = "{{runner}} --oci-runner {{oci_runner}} --docker-compose {{docker_compose}} --project-directory {project_directory} {service} \"$@\"".format(
            project_directory = ctx.file.original_docker_compose.dirname,
            service = ctx.label.name,
        ),
    )

_bzd_oci_json_binary = rule(
    doc = "OCI binary.",
    implementation = _bzd_oci_json_binary_impl,
    attrs = {
        "data": attr.label_list(
            allow_files = True,
            doc = "Additional data to be included in the binary.",
        ),
        "docker_compose": attr.label(
            mandatory = True,
            allow_single_file = ["json"],
            doc = "The name of the Docker image to use.",
        ),
        "original_docker_compose": attr.label(
            allow_single_file = ["yaml", "yml"],
            mandatory = True,
            doc = "The original docker-compose.yaml file.",
        ),
        "_runner": attr.label(
            default = Label("//apps/external:runner"),
            executable = True,
            cfg = "target",
        ),
    },
    toolchains = ["@bzd_rules_oci//toolchain:toolchain_type"],
    executable = True,
)

def _bzd_oci_binary_impl(name, visibility, docker_compose, **kwargs):
    bzd_config_default(
        name = "{}.config".format(name),
        srcs = [
            docker_compose,
        ],
    )

    _bzd_oci_json_binary(
        name = name,
        visibility = visibility,
        original_docker_compose = docker_compose,
        docker_compose = "{}.config.json".format(name),
        **kwargs
    )

bzd_oci_binary = macro(
    inherit_attrs = _bzd_oci_json_binary,
    attrs = {
        "docker_compose": attr.label(
            allow_single_file = ["yaml", "yml"],
            mandatory = True,
            doc = "The docker-compose.yaml file associated with this instance.",
            configurable = False,
        ),
        "original_docker_compose": None,
    },
    implementation = _bzd_oci_binary_impl,
)
