"""Manage configuration for the sanitizer."""

load("@bzd_lib//:repository.bzl", "generate_repository")

def _make_clang_tidy_config(clang_tidy_config):
    return """
alias(
    name = "clang_tidy",
    actual = "{}",
    visibility = ["//visibility:public"],
)
""".format(clang_tidy_config.binary)

def _sanitizer_impl(module_ctx):
    clang_tidy_config = None
    for mod in module_ctx.modules:
        for clang_tidy in mod.tags.clang_tidy:
            if clang_tidy_config != None:
                fail("The clang-tidy config is set twice.")
            clang_tidy_config = struct(
                binary = clang_tidy.binary,
            )

    build_file = ""
    build_file += _make_clang_tidy_config(clang_tidy_config)

    generate_repository(
        name = "bzd_sanitizer_config",
        files = {
            "BUILD": build_file,
        },
    )

sanitizer = module_extension(
    implementation = _sanitizer_impl,
    tag_classes = {
        "clang_tidy": tag_class(
            attrs = {
                "binary": attr.label(
                    mandatory = True,
                    cfg = "exec",
                    executable = True,
                    doc = "The clang-tidy binary, this must be the same as the one running the test to ensure that include path are correct.",
                ),
            },
        ),
    },
)
