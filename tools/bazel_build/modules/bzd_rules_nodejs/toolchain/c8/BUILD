load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_extern_binary")

bzd_nodejs_extern_binary(
    name = "c8",
    binary = "c8/bin/c8.js",
    packages = [
        "@nodejs_deps//:c8",
    ],
    params = [
        "--reporter",
        "lcov",
        "--reporter",
        "text",
        "--allowExternal",
    ],
    visibility = ["//visibility:public"],
)
