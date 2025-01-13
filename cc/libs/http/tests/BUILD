load("//cc/bdl:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "tests",
    srcs = [
        "http.cc",
        "//cc/libs/http/tests/data:request_http1.1_chunked.hh",
        "//cc/libs/http/tests/data:request_http1.1_chunked_expected.hh",
    ],
    deps = [
        "//cc:bzd",
        "//cc/bzd/test",
        "//cc/bzd/utility/ranges/views",
        "//cc/bzd/utility/ranges/views_async",
        "//cc/components/generic/network/tcp/tests:mock",
        "//cc/libs/http",
    ],
)
