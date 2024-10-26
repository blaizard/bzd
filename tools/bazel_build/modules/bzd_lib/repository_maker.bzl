"""Helper to generate a repository made of some files."""

def _repository_maker_impl(repository_ctx):
    # Create files from their content.
    for path, content in repository_ctx.attr.create.items():
        repository_ctx.file(path, content)

    # Download files.
    for path, data_serialized in repository_ctx.attr.download.items():
        data = json.decode(data_serialized)
        repository_ctx.download(
            output = path,
            **data
        )

    # Copy files.
    for path, file in repository_ctx.attr.copy.items():
        repository_ctx.symlink(file, path)

_repository_maker = repository_rule(
    implementation = _repository_maker_impl,
    doc = "Generate a repository.",
    attrs = {
        "copy": attr.string_keyed_label_dict(
            doc = "Files to be copied.",
        ),
        "create": attr.string_dict(
            doc = "Files to be added to the repository with their content. The key is the file name, the value is the content.",
        ),
        "download": attr.string_dict(
            doc = "Files to be downloaded.",
        ),
    },
)

def repository_maker(name, create = None, download = None, copy = None):
    """Generate a repository."""

    _repository_maker(
        name = name,
        create = create,
        download = {k: json.encode(v) for k, v in download.items()} if download else None,
        copy = copy,
    )
