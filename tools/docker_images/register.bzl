load("@io_bazel_rules_docker//container:container.bzl", "container_pull")

def docker_images_register():
    container_pull(
        name = "docker_nodejs",
        digest = "sha256:4df7b8ea856a4b272d5682390f0910fe463b51080d0399302753a83cead49760",
        registry = "index.docker.io",
        repository = "mhart/alpine-node",
        tag = "slim-13",
    )
    container_pull(
        name = "docker_nodejs_puppeteer",
        digest = "sha256:68c92c4b27229e1a3ab27e359fb5474a6edc8cfb572e9b2d46fefc230ead7f65",
        registry = "index.docker.io",
        repository = "blaizard/nodejs_puppeteer",
        tag = "latest",
    )
