load("//tools/bazel.build/toolchains:defs.bzl", "app_toolchain")

toolchain_type(
	name = "toolchain_type",
	visibility = ["//visibility:public"],
)

sh_binary(
    name = "prepare",
    srcs = [
		"bin/prepare.sh"
	],
	visibility = ["//visibility:public"],
)

sh_binary(
    name = "deploy",
    srcs = [
		"bin/deploy.sh"
	],
	visibility = ["//visibility:public"],
)

# Default app toolchain

app_toolchain(
    name = "app",
)

toolchain(
    name = "app_toolchain",
    exec_compatible_with = [
        "@platforms//os:linux",
    ],
    target_compatible_with = [
        "@platforms//os:linux",
    ],
    toolchain = ":app",
    toolchain_type = ":toolchain_type",
)
