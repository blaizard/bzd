"""Make doxygen available."""

load("@bzd_lib//:defs.bzl", "bzd_http_archive", "bzd_repository_multiplatform_maker")

def _doxygen_impl(_module_ctx):
    """Make doxygen available."""

    bzd_repository_multiplatform_maker(
        name = "doxygen",
        repositories = [{
            "build_file": Label("//doxygen:doxygen.BUILD.bazel"),
            "compatible_with": [
                Label("@bzd_platforms//al_isa:linux-x86_64"),
            ],
            "http": [
                bzd_http_archive(
                    integrity = "sha256-PHABHhV2hoT5aDP3PjO+5MKMyT/CS6qHtv+gEbjGn/4=",
                    strip_prefix = "linux_x86_64_1_13_2",
                    url = "https://data.blaizard.com/file/bzd/doxygen/linux_x86_64_1_13_2.tar.xz",
                ),
            ],
        }],
        expose = {
            "doxygen": "doxygen",
        },
    )

doxygen = module_extension(
    implementation = _doxygen_impl,
)
