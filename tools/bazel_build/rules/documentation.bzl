load("//tools/bazel_build:binary_wrapper.bzl", "sh_binary_wrapper_impl")

_DocumentationProvider = provider(
    doc = "Provider for documentation entities.",
    fields = {
        "files": "The input files associated with this documentation.",
        "navigation": "The navigation map.",
    },
)

_COMMON_ATTRS = {
    "srcs": attr.label_list(
        doc = "Source files associated with this documentation",
        allow_files = True,
    ),
    "titles": attr.string_list(
        doc = "Titles associated with each source file.",
    ),
    "data": attr.label_list(
        doc = "Extra files used in the documentation.",
        allow_files = True,
    ),
}

def _doc_preprocess(ctx, files, deps):
    return files

def _doc_library_impl(ctx):
    if len(ctx.attr.srcs) != len(ctx.attr.titles):
        fail("There is not enough sources/titles pair defined.")

    # Create the navigation map and gather dependencies.
    navigation = []
    markdowns = []
    transitive_deps = []
    for index in range(0, len(ctx.attr.srcs)):
        name = ctx.attr.titles[index]
        src = ctx.attr.srcs[index]
        if _DocumentationProvider in src:
            if not name:
                name = src.label.name
            navigation.append((name, src[_DocumentationProvider].navigation))
            transitive_deps.append(src[_DocumentationProvider].files)
        elif src.files:
            for f in src.files.to_list():
                if not name:
                    name = f.basename[:-(len(f.extension) + 1)]
                navigation.append((name, f.path))
            markdowns += src.files.to_list()
        else:
            fail("Input source file '{}', not supported.".format(src))

    deps = depset(ctx.files.data, transitive = transitive_deps)
    preprocessed_markdowns = _doc_preprocess(ctx, markdowns, deps)
    files = depset(preprocessed_markdowns, transitive = [deps])

    return _DocumentationProvider(files = files, navigation = navigation)

_doc_library = rule(
    implementation = _doc_library_impl,
    attrs = _COMMON_ATTRS,
    doc = "Create a documentation library.",
)

def _doc_binary_impl(ctx):
    provider = _doc_library_impl(ctx)
    package = ctx.actions.declare_file("{}.tar".format(ctx.label.name))
    ctx.actions.run(
        inputs = provider.files,
        outputs = [package],
        tools = [ctx.executable._builder, ctx.executable._mkdocs],
        executable = ctx.executable._builder,
        arguments = ["--navigation", json.encode(provider.navigation), "--mkdocs", ctx.executable._mkdocs.path, package.path],
    )

    return [
        sh_binary_wrapper_impl(
            ctx = ctx,
            binary = ctx.attr._web_server,
            output = ctx.outputs.executable,
            command = "{{binary}} $@ \"{}\"".format(package.short_path),
            extra_runfiles = [package],
        ),
        provider,
    ]

_doc_binary = rule(
    implementation = _doc_binary_impl,
    attrs = dict({
        "_builder": attr.label(
            executable = True,
            cfg = "exec",
            default = Label("//tools/bazel_build/rules/assets/documentation:builder"),
        ),
        "_mkdocs": attr.label(
            executable = True,
            cfg = "exec",
            default = Label("//tools/bazel_build/rules/assets/documentation:mkdocs_wrapper"),
        ),
        "_web_server": attr.label(
            executable = True,
            cfg = "target",
            allow_files = True,
            default = Label("//tools/scripts:web_server"),
        ),
    }, **_COMMON_ATTRS),
    doc = "Create a documentation binary.",
    executable = True,
)

def _get_title_source_pair(item):
    """Return the pair title/source from an item."""

    if type(item) == type(""):
        return ("", item)
    if len(item) == 1:
        return ("", item[0])
    if len(item) == 2:
        return item
    fail("Unsupported item '{}' passed to the `srcs` attribute.".format(item))

def doc_library(srcs = None, data = None, **kwargs):
    """Create a documentation library.

    This is a lightweight rule that exposes a provider to be used by `doc_binary` rules.

    Args:
        srcs: A list of pair `title`/`source` or simply `source` associated with this documentation unit.
        data: Additional data that are needed for the documentation.
    """
    _doc_library(
        srcs = [_get_title_source_pair(item)[1] for item in srcs],
        titles = [_get_title_source_pair(item)[0] for item in srcs],
        data = data,
        **kwargs
    )

def doc_binary(srcs = None, data = None, **kwargs):
    """Create a documentation binary.

    When built, this rule will generate the documentation and package it into a tar file with the same name as the rule.
    When run, this rule will spawn a webserver exposing the content of the package.

    Args:
        srcs: A list of pair `title`/`source` or simply `source` associated with this documentation unit.
        data: Additional data that are needed for the documentation.
    """
    _doc_binary(
        srcs = [_get_title_source_pair(item)[1] for item in srcs],
        titles = [_get_title_source_pair(item)[0] for item in srcs],
        data = data,
        **kwargs
    )
