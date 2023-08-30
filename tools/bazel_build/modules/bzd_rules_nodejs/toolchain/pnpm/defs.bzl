"""pnpm binary."""

_BINARIES = {
    "pnpm-linux-x86": struct(
        url = "https://github.com/pnpm/pnpm/releases/download/v8.6.12/pnpm-linuxstatic-x64",
        sha256 = "7e47fa9c8b442d6bb7574b850fad1b77dbdfc8bd7821ee85957dafbe77497f23",
        platforms = [
            "linux-x86_64",
        ],
    ),
}

def _pnpm_install_binary_impl(repository_ctx):
    repository_ctx.download(
        url = repository_ctx.attr.url,
        sha256 = repository_ctx.attr.sha256,
        output = "pnpm",
        executable = True,
    )

    repository_ctx.file(
        "BUILD",
        executable = False,
        content = """
exports_files(["pnpm"])
""",
    )

_pnpm_install_binary = repository_rule(
    doc = "Fetch a specific pnpm binary and store it in its own repository.",
    implementation = _pnpm_install_binary_impl,
    attrs = {
        "sha256": attr.string(),
        "url": attr.string(),
    },
)

def _pnpm_install_impl(repository_ctx):
    select = {}
    for name, data in _BINARIES.items():
        for platform in data.platforms:
            select["@bzd_platforms//al_isa:{}".format(platform)] = "@{}//:pnpm".format(name)

    # Create the BUILD file.
    repository_ctx.file(
        "BUILD",
        executable = False,
        content = """
load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper")

sh_binary_wrapper(
    name = "pnpm",
    binary = select({{
        {select}
    }}),
    command = "XDG_CONFIG_HOME={path} {{binary}} $@",
    visibility = ["//visibility:public"],
)
""".format(
            select = "\n".join(["\"{}\": \"{}\",".format(k, v) for k, v in select.items()]),
            path = repository_ctx.path("."),
        ),
    )

    # Set the configuration, see: https://pnpm.io/cli/config
    # About shamefully-hoist, see: https://github.com/vaadin/flow/issues/9834
    # Hoist cannot be used because we use node with preserve-symlinks which make hoisting not working.
    repository_ctx.file(
        "pnpm/rc",
        content = """
store-dir={store_dir}
shamefully-hoist=true
        """.format(
            store_dir = "/tmp/pnpm",
        ),
        executable = False,
    )

_pnpm_install = repository_rule(
    doc = "Create a repository to aggregate all binaries.",
    implementation = _pnpm_install_impl,
    attrs = {},
)

def pnpm_install():
    """Install the pnpm tool.

    Make it available at @pnpm//:pnpm
    """

    for name, data in _BINARIES.items():
        _pnpm_install_binary(
            name = name,
            url = data.url,
            sha256 = data.sha256,
        )

    _pnpm_install(
        name = "pnpm",
    )
