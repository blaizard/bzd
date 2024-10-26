"""Manage configuration for the sanitizer."""

load("@bzd_lib//:repository_maker.bzl", "repository_maker")

def _make_clang_tidy_config(clang_tidy_config):
    return """
genrule(
    name = "clang_tidy_defaults",
    srcs = [],
    outs = ["no_clang_tidy"],
    cmd = "echo 'No binary were configured for clang-tidy, please use the module extensions.bzl to configure clang-tidy.'; echo 'exit 1' > $@",
)
alias(
    name = "clang_tidy",
    actual = "{}",
    visibility = ["//visibility:public"],
)
alias(
    name = "clang_tidy_config",
    actual = "{}",
    visibility = ["//visibility:public"],
)
""".format(clang_tidy_config.binary if clang_tidy_config else ":no_clang_tidy", clang_tidy_config.config if clang_tidy_config else ":no_clang_tidy")

def _make_mypy_config(mypy_config):
    return """
genrule(
    name = "mypy_defaults",
    srcs = [],
    outs = ["no_mypy"],
    cmd = "echo 'No binary were configured for mypy, please use the module extensions.bzl to configure mypy.'; echo 'exit 1' > $@",
)
alias(
    name = "mypy",
    actual = "{}",
    visibility = ["//visibility:public"],
)
alias(
    name = "mypy_config",
    actual = "{}",
    visibility = ["//visibility:public"],
)
""".format(mypy_config.binary if mypy_config else ":no_mypy", mypy_config.config if mypy_config else ":no_mypy")

def _sanitizer_impl(module_ctx):
    clang_tidy_config = None
    mypy_config = None
    for mod in module_ctx.modules:
        for clang_tidy in mod.tags.clang_tidy:
            if clang_tidy_config != None:
                fail("The clang-tidy config is set twice.")
            clang_tidy_config = struct(
                binary = clang_tidy.binary,
                config = clang_tidy.config,
            )
        for mypy in mod.tags.mypy:
            if mypy_config != None:
                fail("The mypy config is set twice.")
            mypy_config = struct(
                binary = mypy.binary,
                config = mypy.config,
            )

    build_file = ""
    build_file += _make_clang_tidy_config(clang_tidy_config)
    build_file += _make_mypy_config(mypy_config)

    repository_maker(
        name = "bzd_sanitizer_config",
        create = {
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
                "config": attr.label(
                    mandatory = True,
                    doc = "The .clang-tidy configuration.",
                ),
            },
        ),
        "mypy": tag_class(
            attrs = {
                "binary": attr.label(
                    mandatory = True,
                    cfg = "exec",
                    executable = True,
                    doc = "The mypy binary, this must be the same as the one running the test.",
                ),
                "config": attr.label(
                    mandatory = True,
                    doc = "The mypy ini configuration.",
                ),
            },
        ),
    },
)
