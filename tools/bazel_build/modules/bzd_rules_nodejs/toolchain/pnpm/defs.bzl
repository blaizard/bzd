"""Make pnpm tool available."""

load("@bzd_lib//:repository_multiplatform_maker.bzl", "repository_multiplatform_maker")

def pnpm_install(name):
    """Install the pnpm tool.

    Make it available at @pnpm//:pnpm
    """
    repository_multiplatform_maker(
        name = name,
        repositories = [{
            "build_file": Label("//toolchain/pnpm:pnpm.BUILD"),
            "files": {
                "pnpm": {
                    "executable": True,
                    "sha256": "",
                    "url": "https://github.com/pnpm/pnpm/releases/download/v9.12.3/pnpm-linuxstatic-x64",
                },
            },
            "platforms": [
                "linux-x86_64",
            ],
        }, {
            "build_file": Label("//toolchain/pnpm:pnpm.BUILD"),
            "files": {
                "pnpm": {
                    "executable": True,
                    "sha256": "",
                    "url": "https://github.com/pnpm/pnpm/releases/download/v9.12.3/pnpm-linuxstatic-arm64",
                },
            },
            "platforms": [
                "linux-arm64",
            ],
        }],
        expose = {
            "pnpm": "pnpm",
        },
    )
