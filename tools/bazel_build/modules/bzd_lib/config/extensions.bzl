"""Module extension for configurations."""

load("@bazel_skylib//lib:sets.bzl", "sets")
load("//:repository.bzl", "generate_repository")

def _make_defs_bzl(defaults):
    """Generate the content of defs.bzl.

    This contains a CONFIG_DEFAULTS constant with the list of target that can be overwritten.
    """

    content = """
# Contains the list of config labels that can be overwritten.
CONFIG_DEFAULTS = [
{}
]
""".format(",\n".join(["    \"{}\"".format(str(label)) for label in defaults]))
    return content

def _config_impl(module_ctx):
    # Collect all the default labels.
    defaults = sets.make()
    for mod in module_ctx.modules:
        for register in mod.tags.register:
            for label in register.defaults:
                sets.insert(defaults, label.same_package_label("{}.file".format(label.name)))
    defaults = sets.to_list(defaults)

    generate_repository(
        name = "config_defaults",
        files = {
            "BUILD": "",
            "defs.bzl": _make_defs_bzl(defaults),
        },
    )

config = module_extension(
    implementation = _config_impl,
    tag_classes = {
        "register": tag_class(
            attrs = {
                "defaults": attr.label_list(
                    mandatory = False,
                    doc = "List of default configuration that can be overwritten.",
                ),
            },
        ),
    },
)
