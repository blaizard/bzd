load("@bzd_toolchain_nodejs//nodejs:defs.bzl", "bzd_nodejs_extern_binary")

exports_files(["tsconfig.json"])

bzd_nodejs_extern_binary(
    name = "tsc",
    binary = "typescript/bin/tsc",
    packages = {
        "@types/node": "20.5.3",
        "typescript": "5.1.6",
    },
    params = [
        "--typeRoots",
        "{node_modules}/@types",
    ],
    visibility = ["//visibility:public"],
)
