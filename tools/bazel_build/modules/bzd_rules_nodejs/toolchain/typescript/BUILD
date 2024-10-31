load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_extern_binary")

exports_files(["tsconfig.json"])

bzd_nodejs_extern_binary(
    name = "tsc",
    binary = "typescript/bin/tsc",
    packages = [
        "@bzd_rules_nodejs_deps//:typescript",
        "@bzd_rules_nodejs_deps//:typesnode",
    ],
    params = [
        "--typeRoots",
        "{node_modules}/@types",
    ],
    visibility = ["//visibility:public"],
)
