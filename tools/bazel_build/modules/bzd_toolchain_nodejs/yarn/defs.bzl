"""Yarn toolchain."""

yarn = {
    "linux-x86_64": {
        "build_file": "@bzd_toolchain_nodejs//yarn:linux_x86_64.BUILD",
        "sha256": "7e433d4a77e2c79e6a7ae4866782608a8e8bcad3ec6783580577c59538381a6e",
        "strip_prefix": "yarn-v1.22.10",
        "urls": [
            "https://github.com/yarnpkg/yarn/releases/download/v1.22.10/yarn-v1.22.10.tar.gz",
        ],
    },
}
