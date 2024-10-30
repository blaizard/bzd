load("@rules_python//python:defs.bzl", "py_test")

py_test(
    name = "dict",
    srcs = ["dict_test.py"],
    main = "dict_test.py",
    visibility = ["//visibility:public"],
    deps = [
        "@bzd_python//bzd/utils:dict",
    ],
)

py_test(
    name = "match",
    srcs = ["match_test.py"],
    main = "match_test.py",
    visibility = ["//visibility:public"],
    deps = [
        "@bzd_python//bzd/utils:match",
    ],
)

py_test(
    name = "run",
    srcs = ["run_test.py"],
    main = "run_test.py",
    visibility = ["//visibility:public"],
    deps = [
        "@bzd_python//bzd/utils:run",
    ],
)

py_test(
    name = "semver",
    srcs = ["semver_test.py"],
    main = "semver_test.py",
    visibility = ["//visibility:public"],
    deps = [
        "@bzd_python//bzd/utils:semver",
    ],
)

py_test(
    name = "worker",
    srcs = ["worker_test.py"],
    main = "worker_test.py",
    visibility = ["//visibility:public"],
    deps = [
        "@bzd_python//bzd/utils:worker",
    ],
)
