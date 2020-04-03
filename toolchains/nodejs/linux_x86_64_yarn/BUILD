load("//tools/bazel.build:binary_wrapper.bzl", "sh_binary_wrapper")

sh_binary_wrapper(
    name = "yarn",
    binary = "@yarn//:yarn",
    command = "export PATH=$(pwd)/${{RUNFILES_DIR}}/node:$PATH && {binary} --color=always $@",
    data = [
        "@node//:node",
    ],
    visibility = ["//visibility:public"],
)
