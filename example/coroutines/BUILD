load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "coroutines",
	srcs = [
		"main.cc",
	],
    tags = ["manual"],
	copts = [
		"-std=c++2a",
		"-stdlib=libc++",
		"-fcoroutines-ts",
	],
    deps = [
        "//cc/bzd",
    ],
)