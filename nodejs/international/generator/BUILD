load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_binary")

bzd_nodejs_binary(
    name = "generator",
    srcs = [
        "generator.mjs",
    ],
    data = glob([
        "flags/*.svg",
    ]),
    main = "generator.mjs",
    packages = [
        "@nodejs_deps//:jimp",
        "@nodejs_deps//:svg2img",
    ],
    # Somehow this fails from time to time in CI.
    tags = ["manual"],
)
