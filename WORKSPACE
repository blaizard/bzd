load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# CC rules

http_archive(
    name = "rules_cc",
    sha256 = "7e5fd5d3b54217ee40d8488bbd70840456baafb3896782942ae4db1400a77657",
    strip_prefix = "rules_cc-cd7e8a690caf526e0634e3ca55b10308ee23182d",
    urls = ["https://github.com/bazelbuild/rules_cc/archive/cd7e8a690caf526e0634e3ca55b10308ee23182d.zip"],
)

# Toolchains

load("//toolchains/linux_x86_64_clang:defs.bzl", "load_linux_x86_64_clang")
load("//toolchains/esp32_xtensa_lx6_gcc:defs.bzl", "load_esp32_xtensa_lx6_gcc")

load_linux_x86_64_clang()

load_esp32_xtensa_lx6_gcc()

register_toolchains(
    "//tools/bazel.build/toolchains:app_toolchain",
)

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
