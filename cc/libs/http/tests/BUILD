load("//cc/bdl:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "tests",
    srcs = [
        "http.cc",
        "//cc/libs/http/tests/data:request_http1.1_chunked.hh",
    ],
    deps = [
        "//cc:bzd",
        "//cc/bzd/test",
        "//cc/components/generic/network/tcp/tests:mock",
        "//cc/libs/http",
    ],
)
