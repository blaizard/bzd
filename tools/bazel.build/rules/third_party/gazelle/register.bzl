load("@bazel_gazelle//:deps.bzl", "gazelle_dependencies")

def bazel_rules_gazelle_register():
    gazelle_dependencies()
