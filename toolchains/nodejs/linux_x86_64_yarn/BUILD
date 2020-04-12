load("//tools/bazel.build:binary_wrapper.bzl", "sh_binary_wrapper")
load("//tools/bazel.build/toolchains/nodejs:defs.bzl", "nodejs_toolchain")

config_setting(
    name = "cache",
    define_values = {
        "nodejs_cache": "yes",
    },
    visibility = ["//visibility:public"],
)

sh_binary_wrapper(
    name = "yarn",
    binary = "@yarn//:yarn",
    command = select({
        ":cache": "export PATH=$(pwd)/${{RUNFILES_DIR}}/node:$PATH && {binary} --color=always --cache-folder \"/cache/yarn\" --mutex file:\"/cache/.yarn_mutex\" $@",
        "//conditions:default": "export PATH=$(pwd)/${{RUNFILES_DIR}}/node:$PATH && {binary} --color=always $@",
    }),
    data = [
        "@node",
    ],
    visibility = ["//visibility:public"],
)

nodejs_toolchain(
    name = "nodejs_toolchain",
    manager = ":yarn",
    node = "@node//:node",
)

toolchain(
    name = "toolchain",
    exec_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
    ],
    target_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
    ],
    toolchain = ":nodejs_toolchain",
    toolchain_type = "//tools/bazel.build/toolchains/nodejs:toolchain_type",
)
