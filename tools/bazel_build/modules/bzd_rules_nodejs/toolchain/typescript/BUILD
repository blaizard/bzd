load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_extern_binary")

exports_files(["tsconfig.json"])

bzd_nodejs_extern_binary(
    name = "tsc",
    binary = "typescript/bin/tsc",
    packages = {
        "@types/node": "^20.14.6",
        "typescript": "^5.5.2",
    },
    params = [
        "--typeRoots",
        "{node_modules}/@types",
    ],
    visibility = ["//visibility:public"],
)
