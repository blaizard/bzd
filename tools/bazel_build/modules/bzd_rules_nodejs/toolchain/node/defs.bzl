"""Make pnpm tool available."""

load("@bzd_lib//:repository_multiplatform_maker.bzl", "repository_multiplatform_maker")

_repositories = {
    "20.11.0": [{
        "archive": {
            "build_file": Label("//toolchain/node:node.BUILD"),
            "integrity": "sha256-gieANp0Oownn0hjkHeu9GgP4zfNU6/ikQg6J85zC5hI=",
            "strip_prefix": "node-v20.11.0-linux-x64",
            "urls": [
                "https://nodejs.org/dist/v20.11.0/node-v20.11.0-linux-x64.tar.xz",
            ],
        },
        "platforms": [
            "linux-x86_64",
        ],
    }],
    "20.5.1": [{
        "archive": {
            "build_file": Label("//toolchain/node:node.BUILD"),
            "sha256": "a4a700bbca51ac26538eda2250e449955a9cc49638a45b38d5501e97f5b020b4",
            "strip_prefix": "node-v20.5.1-linux-x64",
            "urls": [
                "https://nodejs.org/dist/v20.5.1/node-v20.5.1-linux-x64.tar.xz",
            ],
        },
        "platforms": [
            "linux-x86_64",
        ],
    }],
}

def node_install(version):
    """Install the node tool.

    Make it available at @node-<version>//:node
    """

    repository_multiplatform_maker(
        name = "node-" + version,
        repositories = _repositories[version],
        expose = {
            "node": "node",
        },
    )
