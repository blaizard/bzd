"""Extension for the config module."""

load("//:defs.bzl", "bzd_repository_maker")
load("//private/secret/toolchain/age:defs.bzl", "age_install")

def _lib_impl(module_ctx):
    recipients = {}
    for mod in module_ctx.modules:
        for data in mod.tags.secrets:
            for uid, key in data.recipients.items():
                if uid in recipients:
                    fail("The key '{}' is set twice.".format(uid))
                recipients[uid] = key

    bzd_repository_maker(
        name = "secrets",
        build_file_content = """
exports_files(["recipients.json"])
""",
        files_content = {
            "recipients.json": json.encode_indent(recipients),
        },
    )

    age_install()

lib = module_extension(
    implementation = _lib_impl,
    tag_classes = {
        "secrets": tag_class(attrs = {
            "recipients": attr.string_dict(
                doc = "The identifier and the public key of the recipients.",
            ),
        }),
    },
)
