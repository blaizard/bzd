"""Register rules_docker."""

load("@io_bazel_rules_docker//repositories:deps.bzl", container_deps = "deps")
load("@io_bazel_rules_docker//repositories:repositories.bzl", container_repositories = "repositories")
load("@io_bazel_rules_docker//toolchains/docker:toolchain.bzl", docker_toolchain_configure = "toolchain_configure")

def bazel_rules_docker_register():
    docker_toolchain_configure(
        name = "docker_config",
        gzip_path = "/bin/gzip",
    )

    container_repositories()
    container_deps()
