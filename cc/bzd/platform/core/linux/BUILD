cc_library(
	name = "linux",
	hdrs = [
		"core.h",
	],
	deps = [
		"//cc/bzd/platform:core",
		"//cc/bzd/platform/core:stack",
		"//cc/bzd/container:optional"
	],
	linkopts = [
		"-lpthread",
	]
)

cc_binary(
	name = "main",
	srcs = [
		"main.cc"
	],
	deps = [
		":linux"
	]
)
