load("@rules_python//python:defs.bzl", "py_test")

py_test(
    name = "cbz",
    srcs = ["cbz_test.py"],
    data = [
        ":data/sample1.cbz",
        ":data/sample2.cbz",
        ":data/sample3.cbz",
        ":data/sample4.cbz",
        ":data/sample5.cbz",
        ":data/sample6.cbz",
    ],
    main = "cbz_test.py",
    deps = [
        "//apps/ebook:providers",
        "//apps/ebook/comics:cbz",
    ],
)
