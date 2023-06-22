load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

_repositories = {
    "3.8.16": {
        "linux_x86_64": {
            "urls": ["http://data.blaizard.com/file/bzd/toolchains/python3/linux_x86_64_3.8.16.tar.gz"],
            "strip_prefix": "python",
            "sha256": "afc9953aa0bf9f3a966025a6747e9d65bfb6fb5fc756b95a5d438c948380e431",
        },
    },
}

def _get_constraints(arch):
    if arch == "linux_x86_64":
        return [
            "@platforms//os:linux",
            "@platforms//cpu:x86_64",
        ]
    fail("Unsupported architecture " + arch)

def _toolchain_repository_impl(repository_ctx):
    content = """load("@bazel_skylib//lib:selects.bzl", "selects")
"""
    archs = {}
    for arch in _repositories[repository_ctx.attr.version].keys():
        toolchain_repository = "python_{}_{}".format(repository_ctx.attr.version, arch)
        constraints = _get_constraints(arch)
        content += """
selects.config_setting_group(
    name = "config_{arch}",
    match_all = [{constraints}],
)
toolchain(
    name = "toolchain",
    exec_compatible_with = [":config_{arch}"],
    toolchain = "@{toolchain_repository}//:py_runtime_pair",
    toolchain_type = "@bazel_tools//tools/python:toolchain_type",
)
    """.format(
            arch = arch,
            toolchain_repository = toolchain_repository,
            constraints = ",".join(["\"{}\"".format(c) for c in constraints]),
        )
        archs[arch] = toolchain_repository

    content += """
alias(
    name = "interpreter",
    actual = select({{
        {selects}
    }})
)
""".format(
        selects = ",\n".join(["\":config_{}\": \"@{}//:interpreter\"".format(arch, repo) for arch, repo in archs.items()]),
    )

    repository_ctx.file(
        "BUILD",
        executable = False,
        content = content,
    )

_toolchain_repository = repository_rule(
    implementation = _toolchain_repository_impl,
    local = True,
    attrs = {
        "version": attr.string(),
    },
)

def _toolchain_python_impl(module_ctx):
    http_archive(
        name = "coveragepy",
        sha256 = "47c5b0ce92eaaa3c66086490319987061538b8eedf02d4ef12857fa4c50f284c",
        build_file = "@toolchain_pyton//:coveragepy.BUILD",
        strip_prefix = "coveragepy-4e3cc882db3700912fce5683af37afb11c040768",
        url = "https://github.com/ulfjack/coveragepy/archive/4e3cc882db3700912fce5683af37afb11c040768.zip",
    )

    for mod in module_ctx.modules:
        for toolchain in mod.tags.toolchain:
            repositories = _repositories[toolchain.version]
            for arch, repository in repositories.items():
                http_archive(
                    name = "python_{}_{}".format(toolchain.version, arch),
                    build_file = "@bzd_toolchain_python//:python.BUILD",
                    **repository
                )
            _toolchain_repository(
                name = toolchain.name,
                version = toolchain.version,
            )

toolchain_python = module_extension(
    implementation = _toolchain_python_impl,
    tag_classes = {
        "toolchain": tag_class(
            attrs = {
                "name": attr.string(mandatory = True),
                "version": attr.string(default = "3.8.16", values = _repositories.keys()),
            },
        ),
    },
)
