"""Make bsdtar tool available."""

load("@bzd_lib//:defs.bzl", "bzd_http_file", "bzd_repository_multiplatform_maker")

def tar_install(name):
    """Install the tar tool."""

    bzd_repository_multiplatform_maker(
        name = name,
        repositories = [{
            "build_file": Label("//private/toolchain:tar.BUILD.bazel"),
            "compatible_with": [
                Label("@bzd_platforms//al_isa:linux-x86_64"),
            ],
            "http": [
                bzd_http_file(
                    executable = True,
                    sha256 = "ae24dbc3ecf6ad628c2fdd205a52347fd4446589e750469766669e25cb80e22b",
                    url = "https://github.com/hermeticbuild/bsdtar-prebuilt/releases/download/v3.8.1-3/tar_linux_amd64",
                    output = "tar",
                ),
            ],
        }, {
            "build_file": Label("//private/toolchain:tar.BUILD.bazel"),
            "compatible_with": [
                Label("@bzd_platforms//al_isa:linux-arm64"),
            ],
            "http": [
                bzd_http_file(
                    executable = True,
                    sha256 = "673897c180d9c27770fad1e55914653a6da85fc1f6500df3d768c32916ab8747",
                    url = "https://github.com/hermeticbuild/bsdtar-prebuilt/releases/download/v3.8.1-3/tar_linux_arm64",
                    output = "tar",
                ),
            ],
        }, {
            "build_file": Label("//private/toolchain:tar.BUILD.bazel"),
            "compatible_with": [
                Label("@bzd_platforms//al_isa:osx-arm64"),
            ],
            "http": [
                bzd_http_file(
                    executable = True,
                    sha256 = "f78ae63e48f58be8e88bb1e44ab165503b8bdc7374cebdeb7fa9327629c88907",
                    url = "https://github.com/hermeticbuild/bsdtar-prebuilt/releases/download/v3.8.1-3/tar_darwin_arm64",
                    output = "tar",
                ),
            ],
        }],
        expose = {
            "tar": "tar",
        },
    )
