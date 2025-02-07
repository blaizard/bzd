"""Rules for documentation."""

load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")

DocumentationInfo = provider(
    doc = "Provider for documentation entities.",
    fields = {
        "data": "The data files associated with this documentation.",
        "markdowns": "The markdown files associated with this documentation.",
        "navigation": "The navigation map.",
    },
)

DocumentationExtensionsInfo = provider(
    doc = "Provider for documentation extensions.",
    fields = {
        "data": "Depset of data to be used by the extensions.",
        "json": "Extension files to be used.",
        "tools": "Depset of tools (files_to_run providers) to be used by the extensions.",
    },
)

_COMMON_ATTRS = {
    "data": attr.label_list(
        doc = "Extra files used in the documentation.",
        allow_files = True,
    ),
    "deps": attr.label_list(
        doc = "Dependencies for this documentation.",
        providers = [[DocumentationInfo], [DocumentationExtensionsInfo]],
    ),
    "srcs": attr.label_list(
        doc = "Source files associated with this documentation.",
        allow_files = True,
    ),
    "titles": attr.string_list(
        doc = "Titles associated with each source file.",
    ),
    "_preprocessor": attr.label(
        executable = True,
        cfg = "exec",
        default = Label("//doc:preprocessor"),
    ),
}

def _doc_preprocess(ctx, markdown, deps, extensions):
    # No pre-processing for generated markdown.
    if markdown.path.startswith(ctx.bin_dir.path):
        return markdown

    new_file = ctx.actions.declare_file(markdown.basename, sibling = markdown)

    args = ctx.actions.args()
    args.add_all(extensions.json, before_each = "--extension")
    args.add("--output", new_file)
    args.add(markdown)

    ctx.actions.run(
        inputs = depset(deps + [markdown], transitive = [extensions.data, extensions.json]),
        outputs = [new_file],
        tools = [ctx.attr._preprocessor[DefaultInfo].files_to_run] + extensions.tools.to_list(),
        executable = ctx.executable._preprocessor,
        arguments = [args],
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

def _doc_merge_extensions(deps):
    """Merge all extensions if the provider is available."""

    return DocumentationExtensionsInfo(
        json = depset(transitive = [dep[DocumentationExtensionsInfo].json for dep in deps if DocumentationExtensionsInfo in dep]),
        data = depset(transitive = [dep[DocumentationExtensionsInfo].data for dep in deps if DocumentationExtensionsInfo in dep]),
        tools = depset(transitive = [dep[DocumentationExtensionsInfo].tools for dep in deps if DocumentationExtensionsInfo in dep]),
    )

def _doc_library_impl(ctx):
    if len(ctx.attr.srcs) != len(ctx.attr.titles):
        fail("There is not enough sources/titles pair defined.")

    # Update the dependencies.

    data_from_deps = depset(transitive = [dep[DocumentationInfo].data for dep in ctx.attr.deps if DocumentationInfo in dep])
    extensions = _doc_merge_extensions(ctx.attr.deps)

    # Create the navigation map and gather dependencies.
    navigation = []
    markdowns = []
    providers = []
    for index in range(0, len(ctx.attr.srcs)):
        name = ctx.attr.titles[index]
        src = ctx.attr.srcs[index]
        if DocumentationInfo in src:
            if not name:
                name = src.label.name
            if name == "**":
                navigation.extend(src[DocumentationInfo].navigation)
            else:
                navigation.append((name, src[DocumentationInfo].navigation))
            providers.append(src[DocumentationInfo])
        elif src.files:
            for f in src.files.to_list():
                if not name:
                    name = f.basename[:-(len(f.extension) + 1)]
                markdown = _doc_preprocess(ctx, f, ctx.files.data, extensions = extensions)
                navigation.append((name, markdown.path))
                markdowns.append(markdown)
        else:
            fail("Input source file '{}', not supported.".format(src))

    # Merge extensions coming from the sources.
    all_extensions = _doc_merge_extensions(ctx.attr.deps + ctx.attr.srcs)

    # Symlink the data to make sure it resides in the the same directory as the markdowns.
    local_data = [_doc_preprocess_data(ctx, f) for f in ctx.files.data]
    data = depset(local_data, transitive = [p.data for p in providers] + [data_from_deps])
    markdowns = depset(markdowns, transitive = [p.markdowns for p in providers])

    return [DocumentationInfo(markdowns = markdowns, data = data, navigation = navigation), all_extensions]

_doc_library = rule(
    implementation = _doc_library_impl,
    attrs = _COMMON_ATTRS,
    doc = "Create a documentation library.",
)

def _create_symlinks(ctx, files):
    symlinks = []
    for f in files:
        symlink = ctx.actions.declare_file(f.path)
        ctx.actions.symlink(
            target_file = f,
            output = symlink,
        )
        symlinks.append(symlink)
    return symlinks

def _doc_binary_impl(ctx):
    provider, _ = _doc_library_impl(ctx)
    package = ctx.actions.declare_file("{}.tar".format(ctx.label.name))

    # Create symlink of the js to have the file present under the root tree.
    # This needs to be done at the top level to remove any duplicates.
    js_symlinks = _create_symlinks(ctx, ctx.files._js)

    args = ctx.actions.args()
    args.add_all([f.short_path for f in js_symlinks], before_each = "--js")
    args.add("--navigation", json.encode(provider.navigation))
    args.add("--root", ctx.bin_dir.path)
    args.add("--mkdocs", ctx.executable._mkdocs)
    args.add(package)

    ctx.actions.run(
        inputs = depset(js_symlinks, transitive = [provider.data, provider.markdowns]),
        outputs = [package],
        tools = [ctx.executable._builder, ctx.executable._mkdocs],
        executable = ctx.executable._builder,
        progress_message = "Documentation for {}...".format(ctx.label),
        arguments = [args],
    )

    return [
        sh_binary_wrapper_impl(
            ctx = ctx,
            binary = ctx.attr._web_server,
            output = ctx.outputs.executable,
            command = "{{binary}} $@ \"{}\"".format(package.short_path),
            data = [package],
        ),
        provider,
    ]

_doc_binary = rule(
    implementation = _doc_binary_impl,
    attrs = dict({
        "_builder": attr.label(
            executable = True,
            cfg = "exec",
            default = Label("//doc:builder"),
        ),
        "_js": attr.label_list(
            allow_files = True,
            default = [
                Label("//doc:js/script.js"),
            ],
        ),
        "_mkdocs": attr.label(
            executable = True,
            cfg = "exec",
            default = Label("//doc:mkdocs"),
        ),
        "_web_server": attr.label(
            executable = True,
            cfg = "target",
            default = Label("@bzd_lib//:web_server"),
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
        **kwargs: Additional arguments to be passed to the rule.
    """
    _doc_library(
        srcs = [_get_title_source_pair(item)[1] for item in srcs or []],
        titles = [_get_title_source_pair(item)[0] for item in srcs or []],
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
        **kwargs: Additional arguments to be passed to the rule.
    """
    _doc_binary(
        srcs = [_get_title_source_pair(item)[1] for item in srcs],
        titles = [_get_title_source_pair(item)[0] for item in srcs],
        data = data,
        **kwargs
    )

def _doxygen_library_impl(ctx):
    xml = ctx.actions.declare_directory("{}.doxygen/xml".format(ctx.label.name))
    template = ctx.actions.declare_file("{}.template".format(ctx.label.name))
    json = ctx.actions.declare_file("{}.json".format(ctx.label.name))

    ctx.actions.expand_template(
        template = ctx.file._doxygen_config,
        output = template,
        substitutions = {
            "%INPUTS%": " ".join([src.path for src in ctx.files.srcs]),
            "%OUTPUT%": xml.dirname,
        },
    )

    ctx.actions.run(
        inputs = ctx.files.srcs + [template],
        outputs = [xml],
        executable = ctx.executable._doxygen,
        arguments = [template.path],
        progress_message = "Doxygen for {}...".format(ctx.label),
    )

    ctx.actions.run(
        inputs = [xml],
        outputs = [json],
        executable = ctx.executable._doxygen_parser,
        progress_message = "Doxygen parsing for {}...".format(ctx.label),
        arguments = ["--output", json.path, xml.path],
    )

    return [DefaultInfo(files = depset([json]))]

doc_cc_library = rule(
    implementation = _doxygen_library_impl,
    attrs = {
        "srcs": attr.label_list(
            allow_files = True,
            mandatory = True,
            doc = "The input files.",
        ),
        "_doxygen": attr.label(
            executable = True,
            cfg = "exec",
            default = Label("@doxygen//:doxygen"),
        ),
        "_doxygen_config": attr.label(
            allow_single_file = True,
            default = Label("//doxygen:doxygen.config"),
        ),
        "_doxygen_parser": attr.label(
            executable = True,
            cfg = "exec",
            default = Label("//doxygen:doxygen_parser"),
        ),
    },
)
