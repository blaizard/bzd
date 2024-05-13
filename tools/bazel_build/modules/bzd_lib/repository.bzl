def _generate_repository_impl(repository_ctx):
    for path, content in repository_ctx.attr.files.items():
        repository_ctx.file(path, content)

generate_repository = repository_rule(
    implementation = _generate_repository_impl,
    doc = "Generate a repository.",
    attrs = {
        "files": attr.string_dict(
            doc = "Files to be added to the repository with their content. The key is the file name, the value is the content."
        ),
    },
)
