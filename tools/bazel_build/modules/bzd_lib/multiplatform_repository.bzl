"""Expose target from a multi-platform repository."""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def _multiplatform_repository_impl(repository_ctx):
    build_content = ""

    for exposed_name, target_name in repository_ctx.attr.expose.items():
        build_content += """
alias(
    name = "{exposed_name}",
    actual = select({{
        {select}
    }}),
    visibility = [
        "//visibility:public",
    ]
)
""".format(
            exposed_name = exposed_name,
            select = "\n".join(["\"{}\": \"@{}//:{}\",".format(platform, repository_name, target_name) for platform, repository_name in repository_ctx.attr.platform_mapping.items()]),
        )

    # Create the BUILD file.
    repository_ctx.file(
        "BUILD",
        executable = False,
        content = build_content,
    )

_multiplatform_repository = repository_rule(
    doc = "Create a repository to aggregate all aliases.",
    implementation = _multiplatform_repository_impl,
    attrs = {
        "expose": attr.string_dict(),
        "platform_mapping": attr.string_dict(),
    },
)

def multiplatform_repository(name, repositories, expose):
    """Multi-platform repository.

    Creates a repository that exposes several target which point to target specific underlying repositories.
    This function can be used as follow in a module context:
    ```
    multiplatform_repository(
        name = "my_repo",
        repositories = [{
            "url": "http://data.blaizard.com/file/bzd/qemu/esp32/linux_x86_64_8.1.2.tar.xz",
            "sha256": "df996467865470dbf7c6a80b1d921ffe23eba7be477c165c41192b4343c42d21",
            "build_file": "@bzd_toolchain_cc//:fragments/esp32/qemu/linux_x86_64_8.1.2.BUILD",
            "strip_prefix": "linux_x86_64_8.1.2",
            "platforms": [
                "linux-x86_64",
            ],
        }],
        "expose": {"qemu": "qemu_impl"}
    )
    ```

    Args:
        name: The name of the repository.
        repositories: The platform specific repositories.
        expose: The target to be exposed.
    """

    platform_mapping = {}
    for repository in repositories:
        repository_name = "{}_{}".format(name, "_".join(repository.get("platforms", [])))
        http_archive(
            name = repository_name,
            urls = [
                repository.get("url"),
            ],
            sha256 = repository.get("sha256"),
            strip_prefix = repository.get("strip_prefix"),
            build_file = repository.get("build_file"),
        )
        for platform in repository.get("platforms", []):
            platform_target = "@bzd_platforms//al_isa:{}".format(platform)
            if platform_target in platform_mapping:
                fail("The platform '{}' is defined multiple times.".format(platform_target))
            platform_mapping[platform_target] = repository_name

    _multiplatform_repository(
        name = name,
        platform_mapping = platform_mapping,
        expose = expose,
    )
