"""Make pnpm tool available."""

load("@bzd_lib//:defs.bzl", "bzd_http_file", "bzd_repository_multiplatform_maker")

def pnpm_install(name):
    """Install the pnpm tool.

    Make it available at @pnpm//:pnpm
    """
    bzd_repository_multiplatform_maker(
        name = name,
        repositories = [{
            "build_file": Label("//toolchain/pnpm:pnpm.BUILD.bazel"),
            "compatible_with": [
                Label("@bzd_platforms//al_isa:linux-x86_64"),
            ],
            "http": [
                bzd_http_file(
                    executable = True,
                    sha256 = "",
                    url = "https://github.com/pnpm/pnpm/releases/download/v9.12.3/pnpm-linuxstatic-x64",
                    output = "pnpm",
                ),
            ],
        }, {
            "build_file": Label("//toolchain/pnpm:pnpm.BUILD.bazel"),
            "compatible_with": [
                Label("@bzd_platforms//al_isa:linux-arm64"),
            ],
            "http": [
                bzd_http_file(
                    executable = True,
                    sha256 = "",
                    url = "https://github.com/pnpm/pnpm/releases/download/v9.12.3/pnpm-linuxstatic-arm64",
                    output = "pnpm",
                ),
            ],
        }],
        expose = {
            "pnpm": "pnpm",
        },
    )
