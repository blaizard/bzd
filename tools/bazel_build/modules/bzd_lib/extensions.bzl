"""Extension for the config module."""

load("//:defs.bzl", "bzd_repository_maker")
load("//private/secret/toolchain/age:defs.bzl", "age_install")

def _lib_impl(module_ctx):
    recipients = {}
    required = {}
    for mod in module_ctx.modules:
        for data in mod.tags.recipients:
            for uid, key in data.required.items() + data.optional.items():
                if uid in recipients:
                    fail("The key '{}' is set twice.".format(uid))
                recipients[uid] = key
            required.update(data.required)

    bzd_repository_maker(
        name = "secrets",
        build_file_content = """
exports_files(["recipients.json"])
""",
        files_content = {
            "recipients.json": json.encode_indent({
                "recipients": recipients,
                "required": required.keys(),
            }),
        },
    )

    age_install()

secrets = module_extension(
    implementation = _lib_impl,
    tag_classes = {
        "recipients": tag_class(attrs = {
            "optional": attr.string_dict(
                doc = "The additional identifier and the public key of the recipients, they will be included only if specifically requested.",
            ),
            "required": attr.string_dict(
                doc = "The identifier and the public key of the recipients, they will always be included.",
            ),
        }),
    },
)
