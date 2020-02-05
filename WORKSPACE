load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

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
