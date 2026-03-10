load("@rules_python//python:defs.bzl", "py_binary", "py_library")

py_binary(
    name = "to_json",
    srcs = ["to_json.py"],
    visibility = ["//visibility:public"],
    deps = [
        ":buildstamp_data",
    ],
)

py_library(
    name = "buildstamp_data",
    srcs = ["buildstamp_data.py"],
    visibility = ["//visibility:public"],
)

genrule(
    name = "generate_buildstamp_data",
    outs = ["buildstamp_data.py"],
    cmd = r"""
        echo "BUILDSTAMP = {" > $@
        sed -E 's/([^\s]+)\s+(.*)/"\1": "\2",/' bazel-out/stable-status.txt >> $@
        sed -E 's/([^\s]+)\s+(.*)/"\1": "\2",/' bazel-out/volatile-status.txt >> $@
        echo "}" >> $@
    """,
    stamp = 1,
)
