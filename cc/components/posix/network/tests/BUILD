load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "tests",
    srcs = [
        "address.cc",
        "network.cc",
        "socket.cc",
        "tcp.cc",
    ],
    deps = [
        "//cc:bzd",
        "//cc/bzd/test",
        "//cc/components/posix/network",
        "//cc/components/posix/network/tcp:client",
        "//cc/components/posix/network/tcp:server",
    ],
)
