"""Docker images."""

IMAGES = {
    "bridge": struct(
        digest = "sha256:5b6a59e0c6a28e4dedef5ac2e8a44e2c1327927ede53ae5425923950ac9d99df",
        image = "index.docker.io/blaizard/bridge",
        tag = "latest",
    ),
    "compiler_explorer": struct(
        digest = "sha256:d61369a460589987fa292f7a083720c4057e4b3170cc08e265c6b5cfc6c2eb50",
        image = "index.docker.io/blaizard/compiler_explorer",
        tag = "latest",
    ),
    "nodejs": struct(
        digest = "sha256:4df7b8ea856a4b272d5682390f0910fe463b51080d0399302753a83cead49760",
        image = "index.docker.io/mhart/alpine-node",
        tag = "slim-13",
    ),
    "nodejs_puppeteer": struct(
        digest = "sha256:68c92c4b27229e1a3ab27e359fb5474a6edc8cfb572e9b2d46fefc230ead7f65",
        image = "index.docker.io/blaizard/nodejs_puppeteer",
        tag = "latest",
    ),
    "xtensa_qemu": struct(
        digest = "sha256:c6060b4cfba287f7ca7cb0290d27a8b5f6884d34c744742b5fc9672ed02a81dc",
        image = "index.docker.io/blaizard/xtensa_qemu",
        tag = "latest",
    ),
}
