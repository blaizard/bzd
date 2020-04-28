load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def bazel_rules_cc_load():
    http_archive(
        name = "rules_cc",
        sha256 = "7e5fd5d3b54217ee40d8488bbd70840456baafb3896782942ae4db1400a77657",
        strip_prefix = "rules_cc-cd7e8a690caf526e0634e3ca55b10308ee23182d",
        type = "zip",
        urls = ["https://github.com/bazelbuild/rules_cc/archive/cd7e8a690caf526e0634e3ca55b10308ee23182d.zip"],
    )
