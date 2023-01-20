load("//tools/bazel_build:binary_wrapper.bzl", "sh_binary_wrapper")

genrule(
    name = "build",
    outs = ["docs.tar"],
    srcs = [
        "mkdocs.yml",
        "//:README.md",
    ] + glob([
        "**/*.md",
        "**/*.jpg",
        "**/*.png"
    ]),
    cmd = """
    temp=$$(mktemp -d)
    $(location //tools/documentation:mkdocs_wrapper) -- build --config-file $(location mkdocs.yml) --site-dir "$${temp}"
    tar -cvf $@ -C "$${temp}" .
    rm -rfd "$${temp}"
    """,
    tools = [
        "//tools/documentation:mkdocs_wrapper"
    ],
)

sh_binary_wrapper(
    name = "docs",
    binary = "//tools/scripts:web_server",
    command = """
        {binary} docs/docs.tar "$@"
    """,
    data = [
        "docs.tar",
    ],
    visibility = ["//visibility:public"],
)
