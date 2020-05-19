load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "buildstamp",
    srcs = ["buildstamp.py"],
    visibility = ["//visibility:public"],
    deps = [
        ":buildstamp_data",
    ],
)

py_binary(
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
