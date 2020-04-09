load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Download the rules_docker repository at release v0.14.1
http_archive(
    name = "io_bazel_rules_docker",
    sha256 = "dc97fccceacd4c6be14e800b2a00693d5e8d07f69ee187babfd04a80a9f8e250",
    strip_prefix = "rules_docker-0.14.1",
    urls = ["https://github.com/bazelbuild/rules_docker/releases/download/v0.14.1/rules_docker-v0.14.1.tar.gz"],
)

load("@io_bazel_rules_docker//repositories:repositories.bzl", container_repositories = "repositories")
container_repositories()

load("@io_bazel_rules_docker//repositories:deps.bzl", container_deps = "deps")
container_deps()

load("@io_bazel_rules_docker//container:container.bzl", "container_pull")

container_pull(
    name = "apline_node",
    digest = "sha256:cf8ed910652a097ee152880b7c9988aaad439af91a256a51139938b686b1418d",
    registry = "index.docker.io",
    repository = "mhart/alpine-node",
    tag = "slim-12",
)

# CC rules

http_archive(
    name = "rules_cc",
    sha256 = "7e5fd5d3b54217ee40d8488bbd70840456baafb3896782942ae4db1400a77657",
    strip_prefix = "rules_cc-cd7e8a690caf526e0634e3ca55b10308ee23182d",
    type = "zip",
    urls = ["https://github.com/bazelbuild/rules_cc/archive/cd7e8a690caf526e0634e3ca55b10308ee23182d.zip"],
)

# Debian rules

http_archive(
    name = "debian_repository_rules",
    sha256 = "fc24e3018aa6e331cfa568b6ad28bb2f6851f985270cff2a7364fb4c082cfcda",
    strip_prefix = "debian_repository_rules-3f80031c3ccee36931400323f452f569b8046966",
    type = "zip",
    url = "https://github.com/FaBrand/debian_repository_rules/archive/3f80031c3ccee36931400323f452f569b8046966.zip",
)

# Toolchains

load("//toolchains:workspace.bzl", "load_toolchains")

load_toolchains()

# Buildifier

load("//tools/buildifier:dependencies.bzl", "load_buildifier_dependencies")

load_buildifier_dependencies()

load("//tools/buildifier:buildifier.bzl", "load_buildifier")

load_buildifier()

# Documentation

load("//tools/documentation:dependencies.bzl", "load_documentation_dependencies")

load_documentation_dependencies()

# Sanitizer

load("//tools/sanitizer:dependencies.bzl", "load_sanitizer_dependencies")

load_sanitizer_dependencies()
