load("@io_bazel_rules_go//go:deps.bzl", "go_rules_dependencies", "go_register_toolchains")

def bazel_rules_go_register():
    go_rules_dependencies()
    go_register_toolchains()
