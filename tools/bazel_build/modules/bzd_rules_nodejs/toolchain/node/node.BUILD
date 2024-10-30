load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper")

package(default_visibility = ["//visibility:public"])

exports_files([
    "bin/node",
    "bin/npm",
])

sh_binary(
    name = "node",
    srcs = ["bin/node"],
    data = glob([
        "lib/**/*",
    ]),
)

sh_binary_wrapper(
    name = "npm",
    locations = {
        ":bin/npm": "npm",
        ":bin": "node"
    },
    command = "PATH={node}:$PATH {npm} $@",
    data = glob([
        "lib/**/*",
    ]),
)
