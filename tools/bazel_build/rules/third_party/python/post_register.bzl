load("@python_deps//:requirements.bzl", "install_deps")

def bazel_rules_python_post_register():
    install_deps()
