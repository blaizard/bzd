"""Make doxygen available."""

load("@bzd_lib//:repository_multiplatform_maker.bzl", "repository_multiplatform_maker")

def _doxygen_impl(_module_ctx):
    """Make doxygen available."""

    repository_multiplatform_maker(
        name = "doxygen",
        repositories = [{
            "archive": {
                "build_file": Label("//doxygen:doxygen.BUILD"),
                "integrity": "sha256-PHABHhV2hoT5aDP3PjO+5MKMyT/CS6qHtv+gEbjGn/4=",
                "strip_prefix": "linux_x86_64_1_13_2",
                "urls": [
                    "https://data.blaizard.com/file/bzd/doxygen/linux_x86_64_1_13_2.tar.xz",
                ],
            },
            "platforms": [
                "linux-x86_64",
            ],
        }],
        expose = {
            "doxygen": "doxygen",
        },
    )

doxygen = module_extension(
    implementation = _doxygen_impl,
)
