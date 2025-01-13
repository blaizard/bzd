load("@bzd_lib//embed:defs.bzl", "bzd_embed")

bzd_embed(
    name = "request_http1.1_chunked",
    format = "cc-include",
    input = "request_http1.1_chunked.raw",
    output = "request_http1.1_chunked.hh",
    visibility = ["//cc/libs/http/tests:__pkg__"],
)

bzd_embed(
    name = "request_http1.1_chunked_expected",
    format = "cc-include",
    input = "request_http1.1_chunked_expected.raw",
    output = "request_http1.1_chunked_expected.hh",
    visibility = ["//cc/libs/http/tests:__pkg__"],
)
