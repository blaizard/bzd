load("//nodejs:defs.bzl", "bzd_nodejs_requirements_compile")

exports_files(["compile.py"])

bzd_nodejs_requirements_compile(
    name = "requirements",
    srcs = [
        "requirements.in",
    ],
)
