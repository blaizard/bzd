load("@rules_python//python:defs.bzl", "py_binary")
load(":defs.bzl", "bzd_artifacts_upload")

py_binary(
    name = "upload",
    srcs = [
        "upload.py",
    ],
    deps = [
        "@bzd_python//bzd/http:client",
    ],
)

bzd_artifacts_upload(
    name = "cli",
)
