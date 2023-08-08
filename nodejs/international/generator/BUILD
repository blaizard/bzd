load("@bzd_toolchain_nodejs//nodejs:defs.bzl", "bzd_nodejs_binary")

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
    # Somehow this fails from time to time in CI.
    tags = ["manual"],
)
