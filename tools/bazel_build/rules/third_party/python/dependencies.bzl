load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def bazel_rules_python_load():
    http_archive(
        name = "rules_python",
        strip_prefix = "rules_python-0.0.2",
        url = "https://github.com/bazelbuild/rules_python/releases/download/0.0.2/rules_python-0.0.2.tar.gz",
        sha256 = "b5668cde8bb6e3515057ef465a35ad712214962f0b3a314e551204266c7be90c",
    )
