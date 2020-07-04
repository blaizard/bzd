load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("//tools/sanitizer/actions/buildifier:dependencies.bzl", "buildifier_load")

def sanitizer_load():
    http_archive(
        name = "bazel-compilation-database",
        strip_prefix = "bazel-compilation-database-0.4.0",
        urls = ["https://github.com/grailbio/bazel-compilation-database/archive/0.4.0.zip"],
        sha256 = "a0afa91812b437b1bdaafde12bc777b3d16df267ccc461425beef3c828e05a52",
    )
    buildifier_load()
