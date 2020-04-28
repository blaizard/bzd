load("@io_bazel_rules_docker//repositories:repositories.bzl", "repositories")
load("@io_bazel_rules_docker//repositories:deps.bzl", "deps")

def bazel_rules_docker_register():
    repositories()
    deps()
