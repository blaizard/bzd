load("@utils//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")

_DocumentationProvider = provider(
    doc = "Provider for documentation entities.",
    fields = {
        "markdowns": "The markdown files associated with this documentation.",
        "data": "The data files associated with this documentation.",
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
    "_preprocessor": attr.label(
        executable = True,
        cfg = "exec",
        default = Label("@rules_doc//doc:preprocessor"),
    ),
}

def _doc_preprocess(ctx, markdown, deps):
    # No pre-processing for generated markdown.
    if markdown.path.startswith(ctx.bin_dir.path):
        return markdown

    new_file = ctx.actions.declare_file(markdown.basename, sibling = markdown)
    ctx.actions.run(
        inputs = deps + [markdown],
        outputs = [new_file],
        tools = [ctx.executable._preprocessor],
        executable = ctx.executable._preprocessor,
        arguments = ["--output", new_file.path, markdown.path],
        progress_message = "Preprocessing {}".format(markdown.path),
    )
    return new_file

def _doc_preprocess_data(ctx, data):
    # No pre-processing for generated data.
    if data.path.startswith(ctx.bin_dir.path):
        return data

    new_data = ctx.actions.declare_file(data.basename, sibling = data)
    ctx.actions.symlink(
        target_file = data,
        output = new_data,
    )
    return new_data

def _doc_library_impl(ctx):
    if len(ctx.attr.srcs) != len(ctx.attr.titles):
        fail("There is not enough sources/titles pair defined.")

    # Create the navigation map and gather dependencies.
    navigation = []
    markdowns = []
    providers = []
    for index in range(0, len(ctx.attr.srcs)):
        name = ctx.attr.titles[index]
        src = ctx.attr.srcs[index]
        if _DocumentationProvider in src:
            if not name:
                name = src.label.name
            if name == "**":
                navigation.extend(src[_DocumentationProvider].navigation)
            else:
                navigation.append((name, src[_DocumentationProvider].navigation))
            providers.append(src[_DocumentationProvider])
        elif src.files:
            for f in src.files.to_list():
                if not name:
                    name = f.basename[:-(len(f.extension) + 1)]
                markdown = _doc_preprocess(ctx, f, ctx.files.data)
                navigation.append((name, markdown.short_path))
                markdowns.append(markdown)
        else:
            fail("Input source file '{}', not supported.".format(src))

    # Symlink the data to make sure it resides in the the same directory as the markdowns.
    data = []
    for f in ctx.files.data:
        new_file = _doc_preprocess_data(ctx, f)
        data.append(new_file)

    data = depset(data, transitive = [p.data for p in providers])
    markdowns = depset(markdowns, transitive = [p.markdowns for p in providers])

    return _DocumentationProvider(markdowns = markdowns, data = data, navigation = navigation)

_doc_library = rule(
    implementation = _doc_library_impl,
    attrs = _COMMON_ATTRS,
    doc = "Create a documentation library.",
)

def _doc_binary_impl(ctx):
    provider = _doc_library_impl(ctx)
    package = ctx.actions.declare_file("{}.tar".format(ctx.label.name))

    ctx.actions.run(
        inputs = depset(transitive = [provider.data, provider.markdowns]),
        outputs = [package],
        tools = [ctx.executable._builder, ctx.executable._mkdocs],
        executable = ctx.executable._builder,
        arguments = ["--navigation", json.encode(provider.navigation), "--root", ctx.bin_dir.path, "--mkdocs", ctx.executable._mkdocs.path, package.path],
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
            default = Label("@rules_doc//doc:builder"),
        ),
        "_mkdocs": attr.label(
            executable = True,
            cfg = "exec",
            default = Label("@rules_doc//doc:mkdocs_wrapper"),
        ),
        "_web_server": attr.label(
            executable = True,
            cfg = "target",
            default = Label("@utils//:web_server"),
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
