"""Helper to generate a repository made of some files."""

def bzd_http_archive(**kwargs):
    return json.encode(["http_archive", kwargs])

def bzd_http_file(**kwargs):
    return json.encode(["http_file", kwargs])

def _bzd_repository_maker_impl(repository_ctx):
    # Http content.
    for data_serialized in repository_ctx.attr.http:
        [data_type, kwargs] = json.decode(data_serialized)
        if data_type == "http_file":
            repository_ctx.download(
                **kwargs
            )
        elif data_type == "http_archive":
            repository_ctx.download_and_extract(
                **kwargs
            )
        else:
            fail("Unsupported http type '{}' in {}".format(data_type, data_serialized))

    # Create files with their content.
    for path, content in repository_ctx.attr.files_content.items():
        repository_ctx.file(path, content)
    if repository_ctx.attr.build_file_content:
        repository_ctx.file("BUILD.bazel", repository_ctx.attr.build_file_content)

    # Create symlinks with existing files.
    for path, file in repository_ctx.attr.files.items():
        repository_ctx.symlink(file, path)
    if repository_ctx.attr.build_file:
        repository_ctx.symlink(repository_ctx.attr.build_file, "BUILD.bazel")

bzd_repository_maker = repository_rule(
    implementation = _bzd_repository_maker_impl,
    doc = "Generate a repository.",
    attrs = {
        "build_file": attr.label(
            allow_single_file = True,
            doc = "The file to use as the BUILD file for this repository.",
        ),
        "build_file_content": attr.string(
            doc = "The content for the BUILD file for this repository. Either build_file or build_file_content can be specified, but not both.",
        ),
        "files": attr.string_keyed_label_dict(
            doc = "Files to be copied.",
        ),
        "files_content": attr.string_dict(
            doc = "Files to be added to the repository with their content. The key is the file name, the value is the content.",
        ),
        "http": attr.string_list(
            doc = "Content to be downloaded via http, it only accepts entries from 'bzd_http_archive' and 'bzd_http_file'.",
        ),
    },
)
