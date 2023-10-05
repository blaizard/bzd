"""Rules for NodeJs."""

load("@bzd_lib//lib:attrs.bzl", "ATTRS_COMMON_BINARY_RULES", "ATTRS_COMMON_BUILD_RULES", "ATTRS_COMMON_TEST_RULES", "attrs_assert_any_of")
load("@bzd_rules_nodejs//nodejs:private/nodejs_docker.bzl", bzd_nodejs_docker_ = "bzd_nodejs_docker")
load("@bzd_rules_nodejs//nodejs:private/nodejs_executable.bzl", bzd_nodejs_binary_ = "bzd_nodejs_binary", bzd_nodejs_test_ = "bzd_nodejs_test")
load("@bzd_rules_nodejs//nodejs:private/nodejs_extern_binary.bzl", bzd_nodejs_extern_binary_ = "bzd_nodejs_extern_binary")
load("@bzd_rules_nodejs//nodejs:private/nodejs_install.bzl", bzd_nodejs_install_ = "bzd_nodejs_install")
load("@bzd_rules_nodejs//nodejs:private/nodejs_library.bzl", bzd_nodejs_library_ = "bzd_nodejs_library")
load("@bzd_rules_nodejs//nodejs:private/nodejs_web_binary.bzl", bzd_nodejs_web_binary_ = "bzd_nodejs_web_binary")

# Public API.

bzd_nodejs_docker = bzd_nodejs_docker_
bzd_nodejs_install = bzd_nodejs_install_
bzd_nodejs_library = bzd_nodejs_library_
bzd_nodejs_extern_binary = bzd_nodejs_extern_binary_
bzd_nodejs_web_binary = bzd_nodejs_web_binary_

def bzd_nodejs_binary(name, main, data = [], tools = [], deps = [], packages = {}, srcs = [], apis = [], tags = [], **kwargs):
    attrs_assert_any_of(kwargs, ATTRS_COMMON_BUILD_RULES, ATTRS_COMMON_BINARY_RULES)

    bzd_nodejs_install(
        name = name + ".install",
        data = data,
        deps = deps,
        packages = packages,
        srcs = srcs,
        apis = apis,
        tags = ["nodejs", "manual"],
        tools = tools,
    )

    bzd_nodejs_binary_(
        name = name,
        main = main,
        data = data,
        install = name + ".install",
        tags = ["nodejs"] + tags,
        **kwargs
    )

def bzd_nodejs_test(name, main, data = [], tools = [], deps = [], packages = {}, srcs = [], apis = [], tags = [], **kwargs):
    attrs_assert_any_of(kwargs, ATTRS_COMMON_BUILD_RULES, ATTRS_COMMON_TEST_RULES)

    bzd_nodejs_install(
        name = name + ".install",
        data = data,
        deps = deps,
        packages = packages,
        srcs = srcs,
        apis = apis,
        tags = ["nodejs", "manual"],
        tools = tools,
    )

    bzd_nodejs_test_(
        name = name,
        main = main,
        data = data,
        executor = "@bzd_rules_nodejs//toolchain/mocha",
        install = name + ".install",
        tags = ["nodejs"] + tags,
        **kwargs
    )
