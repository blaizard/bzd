"""Docker images."""

IMAGES = {
    "bridge": {
        "digest": "sha256:3ae86fc21f2a6e5b1b7d26018bdecc8f1801919d7c23d56e93c9311c6a051d40",
        "image": "index.docker.io/blaizard/bridge",
    },
    "compiler_explorer": {
        "digest": "sha256:d61369a460589987fa292f7a083720c4057e4b3170cc08e265c6b5cfc6c2eb50",
        "image": "index.docker.io/blaizard/compiler_explorer",
    },
    "minimal": {
        "digest": "sha256:00a24d7c50ebe46934e31f6154c0434e2ab51259a65e028be42413c636385f7f",
        "image": "index.docker.io/library/debian",
        "platforms": [
            "linux/amd64",
        ],
    },
    "nodejs": {
        "digest": "sha256:1467ea23cce893347696b155b9e00e7f0ac7d21555eb6f27236f1328212e045e",  # 23-alpine
        "image": "index.docker.io/library/node",
        "platforms": [
            "linux/amd64",
        ],
    },
    "nodejs_puppeteer": {
        "digest": "sha256:68c92c4b27229e1a3ab27e359fb5474a6edc8cfb572e9b2d46fefc230ead7f65",
        "image": "index.docker.io/blaizard/nodejs_puppeteer",
    },
    "xtensa_qemu": {
        "digest": "sha256:c6060b4cfba287f7ca7cb0290d27a8b5f6884d34c744742b5fc9672ed02a81dc",
        "image": "index.docker.io/blaizard/xtensa_qemu",
    },
}
