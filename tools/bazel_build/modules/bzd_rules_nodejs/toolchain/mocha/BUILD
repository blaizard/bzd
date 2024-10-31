load("//nodejs:defs.bzl", "bzd_nodejs_extern_binary")

bzd_nodejs_extern_binary(
    name = "mocha",
    binary = "mocha/bin/mocha.js",
    packages = [
        "@bzd_rules_nodejs_deps//:mocha",
    ],
    visibility = ["//visibility:public"],
)
