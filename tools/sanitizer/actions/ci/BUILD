load("@rules_python//python:python.bzl", "py_binary")

py_binary(
    name = "jenkins",
    srcs = [
        "generate.py",
    ],
    args = [
        "--filter",
        "hello",
        "$(location //tools/ci:stages.json)",
        "$(location //tools/ci:jenkins/Jenkinsfile.btl)",
    ],
    data = [
        "//tools/ci:jenkins/Jenkinsfile.btl",
        "//tools/ci:stages.json",
    ],
    main = "generate.py",
    visibility = ["//tools/sanitizer:__pkg__"],
    deps = [
        "//python/bzd/template",
    ],
)
