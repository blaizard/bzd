load("@bazel_skylib//lib:sets.bzl", "sets")
load("@bzd_lib//:repository.bzl", "generate_repository")

def _target_to_py_module(target):
    return target.package.replace("/", ".") + "." + target.name

def _bdl_extension_impl(module_ctx):
    extensions = {}
    deps = sets.make()
    deps_extensions = []
    extensions_content = ""
    py_content_imports = ""
    py_formatters = []
    py_compositions = []
    for mod in module_ctx.modules:
        for register in mod.tags.register:
            if register.name in extensions:
                fail("Extension '{}' is registered twice.".format(register.name))
            extensions[register.name] = True
            if register.format:
                sets.insert(deps, register.format)
                py_content_imports += """from {module} import format as format_{name}\n""".format(
                    module = _target_to_py_module(register.format),
                    name = register.name,
                )
                py_formatters.append("format_{name}".format(name = register.name))
            if register.composition:
                sets.insert(deps, register.composition)
                py_content_imports += """from {module} import composition as composition_{name}\n""".format(
                    module = _target_to_py_module(register.composition),
                    name = register.name,
                )
                py_compositions.append("composition_{name}".format(name = register.name))
            extensions_content += """load("{target}", {name} = "extension")\n""".format(
                target = register.extension,
                name = register.name,
            )
            deps_extensions.append(register.extension)

    extensions_content += """extensions = {}\n""".format(" | ".join(extensions.keys()))

    generate_repository(
        name = "bdl_extension",
        files = {
            "BUILD": """
load("@bazel_skylib//:bzl_library.bzl", "bzl_library")

bzl_library(
    name = "extensions",
    srcs = [
        "extensions.bzl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        {deps_extensions}
    ],
)

py_library(
    name = "bdl_extension",
    srcs = ["bdl_extension.py"],
    visibility = ["@bzd_bdl//:__subpackages__"],
    deps = [
        {deps}
    ]
)
""".format(
                deps_extensions = ",\n".join(["\"" + str(e) + "\"" for e in deps_extensions]),
                deps = ",\n".join(["\"" + str(e) + "\"" for e in sets.to_list(deps)]),
            ),
            "extensions.bzl": extensions_content,
            "bdl_extension.py": py_content_imports + """

formatters = {{}} | {formatters}
compositions = {{}} | {compositions}
""".format(
                formatters = " | ".join(py_formatters),
                compositions = " | ".join(py_compositions),
            ),
        },
    )

bdl_extension = module_extension(
    implementation = _bdl_extension_impl,
    tag_classes = {
        "register": tag_class(
            attrs = {
                "name": attr.string(mandatory = True),
                "extension": attr.label(mandatory = True),
                "format": attr.label(),
                "composition": attr.label(),
            },
        ),
    },
)
