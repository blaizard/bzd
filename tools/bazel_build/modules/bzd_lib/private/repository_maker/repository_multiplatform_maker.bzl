"""Expose target from a multi-platform repository."""

load("//private/repository_maker:repository_maker.bzl", "bzd_repository_maker")

def _repository_multiplatform_maker_impl(repository_ctx):
    build_content = ""
    defs_content = ""

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

        defs_content += """
{exposed_name} = {{
    {content}
}}
""".format(
            exposed_name = exposed_name,
            content = "\n".join(["Label(\"{}\"): Label(\"@{}//:{}\"),".format(platform, repository_name, target_name) for platform, repository_name in repository_ctx.attr.platform_mapping.items()]),
        )

    # Create the BUILD file.
    repository_ctx.file(
        "BUILD",
        executable = False,
        content = build_content,
    )

    # Create the defs.bzl file.
    repository_ctx.file(
        "defs.bzl",
        executable = False,
        content = defs_content,
    )

_repository_multiplatform_maker = repository_rule(
    doc = "Create a repository to aggregate all aliases.",
    implementation = _repository_multiplatform_maker_impl,
    attrs = {
        "expose": attr.string_dict(),
        "platform_mapping": attr.label_keyed_string_dict(),
    },
)

def bzd_repository_multiplatform_maker(name, repositories, expose):
    """Multi-platform repository.

    Creates a repository that exposes several target which point to target specific underlying repositories.
    This function can be used as follow in a module context:
    ```
    repository_multiplatform_maker(
        name = "my_repo",
        repositories = [{
            "http": [
                bzd_http_archive(
                    url = "https://data.blaizard.com/file/bzd/qemu/esp32/linux_x86_64_8.1.2.tar.xz",
                    sha256 = "df996467865470dbf7c6a80b1d921ffe23eba7be477c165c41192b4343c42d21",
                )
            ],
            "build_file": "@bzd_toolchain_cc//:fragments/esp32/qemu/linux_x86_64_8.1.2.BUILD",
            "compatible_with": [
                Label("@bzd_platforms//al_isa:linux-x86_64"),
            ],
        }],
        "expose": {"qemu": "qemu_impl"}
    )
    ```

    Args:
        name: The name of the repository.
        repositories: The platform specific repositories.
        expose: The target to be exposed. The target will be exposes in the BUILD file and in a defs.bzl file.
    """

    platform_mapping = {}
    for index, repository in enumerate(repositories):
        repository_name = "{}_{}".format(name, index)

        if "compatible_with" not in repository:
            fail("Missing 'compatible_with' attribute.")
        repository = dict(repository)
        compatible_with = repository.pop("compatible_with")

        bzd_repository_maker(
            name = repository_name,
            **repository
        )

        for constraint in compatible_with:
            if constraint in platform_mapping:
                fail("The constraint '{}' is defined multiple times.".format(constraint))
            platform_mapping[constraint] = repository_name

    _repository_multiplatform_maker(
        name = name,
        platform_mapping = platform_mapping,
        expose = expose,
    )
