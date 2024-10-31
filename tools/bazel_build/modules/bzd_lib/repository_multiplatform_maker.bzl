"""Expose target from a multi-platform repository."""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("//:repository_maker.bzl", "repository_maker")

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
            select = "\n".join(["\"@bzd_platforms//al_isa:{}\": \"@{}//:{}\",".format(platform, repository_name, target_name) for platform, repository_name in repository_ctx.attr.platform_mapping.items()]),
        )

        defs_content += """
{exposed_name} = {{
    {content}
}}
""".format(
            exposed_name = exposed_name,
            content = "\n".join(["\"{}\": Label(\"@{}//:{}\"),".format(platform, repository_name, target_name) for platform, repository_name in repository_ctx.attr.platform_mapping.items()]),
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
        "platform_mapping": attr.string_dict(),
    },
)

def repository_multiplatform_maker(name, repositories, expose):
    """Multi-platform repository.

    Creates a repository that exposes several target which point to target specific underlying repositories.
    This function can be used as follow in a module context:
    ```
    repository_multiplatform_maker(
        name = "my_repo",
        repositories = [{
            "archive": {
                "url": "https://data.blaizard.com/file/bzd/qemu/esp32/linux_x86_64_8.1.2.tar.xz",
                "sha256": "df996467865470dbf7c6a80b1d921ffe23eba7be477c165c41192b4343c42d21",
                "build_file": "@bzd_toolchain_cc//:fragments/esp32/qemu/linux_x86_64_8.1.2.BUILD",
                "strip_prefix": "linux_x86_64_8.1.2",
            },
            "platforms": [
                "linux-x86_64",
            ],
        }],
        "expose": {"qemu": "qemu_impl"}
    )

    or

    repository_multiplatform_maker(
        name = "my_repo",
        repositories = [{
            "files": {
                "pnpm": {
                    "url": "https://data.blaizard.com/file/bzd/qemu/esp32/linux_x86_64_8.1.2.tar.xz",
                    "sha256": "df996467865470dbf7c6a80b1d921ffe23eba7be477c165c41192b4343c42d21",
                }
            },
            "build_file": "@bzd_toolchain_cc//:fragments/esp32/qemu/linux_x86_64_8.1.2.BUILD",
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
        expose: The target to be exposed. The target will be exposes in the BUILD file and in a defs.bzl file.
    """

    platform_mapping = {}
    for repository in repositories:
        repository_name = "{}_{}".format(name, "_".join(repository.get("platforms", [])))

        if "archive" in repository:
            for attr in ("build_file", "files"):
                if attr in repository:
                    fail("'{}' attribute should not be present if 'archive' is defined.".format(attr))
            http_archive(
                name = repository_name,
                **repository["archive"]
            )

        if "files" in repository:
            for attr in ("archive",):
                if attr in repository:
                    fail("'{}' attribute should not be present if 'files' is defined.".format(attr))
            repository_maker(
                name = repository_name,
                download = repository["files"],
                copy = {
                    "BUILD.bazel": repository["build_file"],
                },
            )

        for platform in repository.get("platforms", []):
            if platform in platform_mapping:
                fail("The platform '{}' is defined multiple times.".format(platform))
            platform_mapping[platform] = repository_name

    _repository_multiplatform_maker(
        name = name,
        platform_mapping = platform_mapping,
        expose = expose,
    )
