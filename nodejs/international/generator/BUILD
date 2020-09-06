load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_binary")

bzd_nodejs_binary(
    name = "generator",
    srcs = [
        "generator.mjs",
    ],
    data = glob([
        "flags/*.svg",
    ]),
    main = "generator.mjs",
    packages = {
        "jimp": "",
        "svg2img": "",
    },
)
