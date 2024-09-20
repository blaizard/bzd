"""mypy aspect rule."""

_PYTHON_EXTENSIONS = ["py", "pyi"]

MyPyInfo = provider(
    "Information on the file processed by the rule.",
    fields = {
        "transitive_sources": "Sources processed by this rule.",
    },
)

def _get_mypy_transitive_sources(deps):
    """Get the mypy transitive source from the depdnencies."""

    return depset(transitive = [dep[MyPyInfo].transitive_sources for dep in deps if MyPyInfo in dep])

def _get_external(file):
    """Return the external repository name if any."""

    if file.root.path:
        path = file.path[len(file.root.path) + 1:]
    else:
        path = file.path
    if path.startswith("external/"):
        return path.split("/")[1]
    return None

def _to_external(path, root = None):
    """Convert a path into an external path."""

    path = "external/" + path
    if root:
        return root + "/" + path
    return path

def _mypy_aspect_impl(target, ctx):
    """Run mypy on the target."""

    # Only deal with python targets
    if not PyInfo in target:
        return []

    py_rule = ctx.rule

    # Instead of reading the 'srcs' attribute to identify which file need to be mypy-ed, we read the PyInfo and get all the
    # srcs from this rule, inputs, transitives and generated.
    # We only check the subset that has need been checked before, this is done with MyPyInfo.
    # This has the advantage to mypy also the generated python files from custom rules.
    # The only requirement is that the dependencies come from the 'deps' attribute.
    all_srcs = target[PyInfo].transitive_sources
    imports = target[PyInfo].imports
    mypy_transitive_srcs = _get_mypy_transitive_sources(py_rule.attr.deps) if hasattr(py_rule.attr, "deps") else depset()

    mypy_srcs = []
    repositories = []
    roots = []
    for src in all_srcs.to_list():
        # Build the list of files to check with mypy.
        if src not in mypy_transitive_srcs.to_list():
            if src.extension in _PYTHON_EXTENSIONS:
                mypy_srcs.append(src)

        # Build the list of available repositories.
        maybe_repository = _get_external(src)
        if maybe_repository:
            repositories.append(_to_external(maybe_repository, src.root.path))

        # Build the list of root path.
        roots.append(src.root.path or ".")

    mypy_info = MyPyInfo(transitive_sources = depset(mypy_srcs, transitive = [mypy_transitive_srcs]))

    # If there are no files, do nothing.
    if len(mypy_srcs) == 0:
        return [mypy_info]

    args = ctx.actions.args()

    # This flag tells mypy that top-level packages will be based in either the
    # current directory, or a member of the 'MYPYPATH' environment variable or
    # 'mypy_path' config option.
    args.add("--explicit-package-bases")

    # Undocumented flags:
    # --bazel: Makes cache files hermetic by using relative paths and setting mtime to zero.
    # --cache-map: Specifies the mapping from the source file to cache files:
    #              --cache-map foo.py foo.meta.json foo.data.json bar.py bar.meta.json bar.data.json
    # --package-root: Specifies alternate root trees.
    args.add("--bazel")
    args.add_all(mypy_srcs)

    # Generate MYPATH as PYTHONPATH is generated:
    # MYPYPATH = imports
    #          += root directory trees (current directory + other roots)
    #          += get repositories imports (list all top directories in external)
    mypy_path = depset([_to_external(path) for path in imports.to_list()] + roots + repositories)

    output = ctx.actions.declare_file("{}.mypy".format(ctx.label.name))
    ctx.actions.run(
        inputs = all_srcs,
        outputs = [output],
        executable = ctx.executable._mypy,
        arguments = [output.path, args],
        env = {
            "MYPYPATH": ":".join(mypy_path.to_list()),
        },
        mnemonic = "Mypy",
        progress_message = "Mypying {}".format(ctx.label),
    )

    return [
        OutputGroupInfo(mypy = depset(direct = [output])),
        mypy_info,
    ]

mypy_aspect = aspect(
    implementation = _mypy_aspect_impl,
    attrs = {
        "_mypy": attr.label(
            doc = "Mypy binary to be used.",
            executable = True,
            cfg = "exec",
            default = Label("//aspects/mypy"),
        ),
    },
    attr_aspects = ["deps"],
)
