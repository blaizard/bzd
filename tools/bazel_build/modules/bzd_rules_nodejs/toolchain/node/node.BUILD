package(default_visibility = ["//visibility:public"])

exports_files(["bin/node"])

sh_binary(
    name = "node",
    srcs = ["bin/node"],
    data = glob([
        "lib/**/*",
    ]),
)
