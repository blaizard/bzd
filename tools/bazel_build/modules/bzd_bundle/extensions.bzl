"""Module extensions for bzd_bundle."""

load("//private/toolchain:tar.bzl", "tar_install")

def _toolchain_tar_impl(_module_ctx):
    tar_install("tar")

toolchain_tar = module_extension(
    implementation = _toolchain_tar_impl,
)
