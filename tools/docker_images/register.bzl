load("//tools/bazel_build/rules:docker.bzl", "bzd_docker_pull")

IMAGES = {
    "bridge": struct(
        digest = "sha256:5b6a59e0c6a28e4dedef5ac2e8a44e2c1327927ede53ae5425923950ac9d99df",
        registry = "index.docker.io",
        repository = "blaizard/bridge",
        tag = "latest",
    ),
    "nodejs": struct(
        digest = "sha256:4df7b8ea856a4b272d5682390f0910fe463b51080d0399302753a83cead49760",
        registry = "index.docker.io",
        repository = "mhart/alpine-node",
        tag = "slim-13",
    ),
    "nodejs_puppeteer": struct(
        digest = "sha256:68c92c4b27229e1a3ab27e359fb5474a6edc8cfb572e9b2d46fefc230ead7f65",
        registry = "index.docker.io",
        repository = "blaizard/nodejs_puppeteer",
        tag = "latest",
    ),
    "xtensa_qemu": struct(
        digest = "sha256:473c71ec94c4fe3acd26f7b9274accabbda73959b6cd062996a8880fc1a2f6b9",
        registry = "index.docker.io",
        repository = "blaizard/xtensa_qemu",
        tag = "latest",
    ),
    "compiler_explorer": struct(
        digest = "sha256:d61369a460589987fa292f7a083720c4057e4b3170cc08e265c6b5cfc6c2eb50",
        registry = "index.docker.io",
        repository = "blaizard/compiler_explorer",
        tag = "latest",
    ),
}

def docker_images_register():
    """Make all docker images accessible via `@docker_image_<name>//:image`."""

    for name, data in IMAGES.items():
        bzd_docker_pull(
            name = "docker_image_{}".format(name),
            digest = data.digest,
            registry = data.registry,
            repository = data.repository,
            tag = data.tag,
        )

def _docker_images_config_impl(ctx):
    config_json = ctx.actions.declare_file(ctx.attr.out.name)
    ctx.actions.write(
        output = config_json,
        content = json.encode(IMAGES),
    )
    return [
        DefaultInfo(files = depset([config_json])),
    ]

docker_images_config = rule(
    doc = """Expose the docker images configuration as a json file.""",
    implementation = _docker_images_config_impl,
    attrs = {
        "out": attr.output(
            doc = "The name of the file created by the rule.",
        ),
    },
)

def docker_execution_platforms():
    native.register_execution_platforms(
        "@//tools/docker_images/linux_x86_64",
    )
