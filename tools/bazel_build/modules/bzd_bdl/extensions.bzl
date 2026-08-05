"""Register a new extension."""

load("@bazel_skylib//lib:sets.bzl", "sets")
load("@bzd_lib//:defs.bzl", "bzd_repository_maker")

def _bdl_extension_impl(module_ctx):
    extensions = {}
    deps = sets.make()
    deps_extensions = []
    extensions_content = ""
    for mod in module_ctx.modules:
        for register in mod.tags.register:
            if register.name in extensions:
                fail("Extension '{}' is registered twice.".format(register.name))
            extensions[register.name] = True
            extensions_content += """load("{target}", {name} = "extension")\n""".format(
                target = register.extension,
                name = register.name,
            )
            deps_extensions.append(register.extension)

    extensions_content += """extensions = {{ {} }}\n""".format(", ".join(["'{}': {}".format(name, name) for name in extensions.keys()]))

    bzd_repository_maker(
        name = "bdl_extension",
        build_file_content = """
load("@bazel_skylib//:bzl_library.bzl", "bzl_library")
load("@rules_python//python:defs.bzl", "py_library")

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
""".format(
            deps_extensions = ",\n".join(["\"" + str(e) + "\"" for e in deps_extensions]),
            deps = ",\n".join(["\"" + str(e) + "\"" for e in sets.to_list(deps)]),
        ),
        files_content = {
            "extensions.bzl": extensions_content,
        },
    )

bdl_extension = module_extension(
    implementation = _bdl_extension_impl,
    tag_classes = {
        "register": tag_class(
            attrs = {
                "composition": attr.label(),
                "extension": attr.label(mandatory = True),
                "name": attr.string(mandatory = True),
            },
        ),
    },
)
