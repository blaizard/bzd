load("@rules_python//python:defs.bzl", "py_binary")

filegroup(
    name = "esp32_bin",
    srcs = glob([
        "bin/esp32/*",
    ]),
)

sh_binary(
    name = "esp32_prepare",
    srcs = [
        "esp32_prepare.sh",
    ],
	data = [
		"@esptool//:tools",
	],
	visibility = ["//visibility:public"],
)

sh_binary(
    name = "esp32_deploy",
    srcs = [
        "esp32_deploy.sh",
    ],
	data = [
		"@esptool//:tools",
		":esp32_bin",
	],
	visibility = ["//visibility:public"],
)
