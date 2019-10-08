load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def load_buildifier_dependencies():
    # buildifier is written in Go and hence needs rules_go to be built.
    # See https://github.com/bazelbuild/rules_go for the up to date setup instructions.
    http_archive(
        name = "io_bazel_rules_go",
        urls = [
            "https://storage.googleapis.com/bazel-mirror/github.com/bazelbuild/rules_go/releases/download/0.19.0/rules_go-0.19.0.tar.gz",
            "https://github.com/bazelbuild/rules_go/releases/download/0.19.0/rules_go-0.19.0.tar.gz",
        ],
        sha256 = "9fb16af4d4836c8222142e54c9efa0bb5fc562ffc893ce2abeac3e25daead144",
    )

    http_archive(
        name = "bazel_gazelle",
        urls = [
            "https://storage.googleapis.com/bazel-mirror/github.com/bazelbuild/bazel-gazelle/releases/download/0.18.1/bazel-gazelle-0.18.1.tar.gz",
            "https://github.com/bazelbuild/bazel-gazelle/releases/download/0.18.1/bazel-gazelle-0.18.1.tar.gz",
        ],
        sha256 = "be9296bfd64882e3c08e3283c58fcb461fa6dd3c171764fcc4cf322f60615a9b",
    )

    http_archive(
        name = "com_google_protobuf",
        strip_prefix = "protobuf-3.10.0",
        urls = ["https://github.com/protocolbuffers/protobuf/releases/download/v3.10.0/protobuf-all-3.10.0.zip"],
		sha256 = "b5dea11d2e35382a1979d1e28a2ea40d06dcaecebc81f45294095429f9249122"
    )

    http_archive(
        name = "com_github_bazelbuild_buildtools",
        strip_prefix = "buildtools-5bcc31df55ec1de770cb52887f2e989e7068301f",
        url = "https://github.com/bazelbuild/buildtools/archive/5bcc31df55ec1de770cb52887f2e989e7068301f.zip",
		sha256 = "875d0c49953e221cfc35d2a3846e502f366dfa4024b271fa266b186ca4664b37"
    )
