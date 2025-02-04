"""Rules for NodeJs."""

load("@bzd_lib//lib:attrs.bzl", "ATTRS_COMMON_BINARY_RULES", "ATTRS_COMMON_BUILD_RULES", "ATTRS_COMMON_TEST_RULES", "attrs_assert_any_of")
load("//nodejs:private/nodejs_executable.bzl", bzd_nodejs_binary_ = "bzd_nodejs_binary", bzd_nodejs_test_ = "bzd_nodejs_test")
load("//nodejs:private/nodejs_extern_binary.bzl", bzd_nodejs_extern_binary_ = "bzd_nodejs_extern_binary")
load("//nodejs:private/nodejs_install.bzl", bzd_nodejs_install_ = "bzd_nodejs_install")
load("//nodejs:private/nodejs_library.bzl", bzd_nodejs_library_ = "bzd_nodejs_library")
load("//nodejs:private/nodejs_oci.bzl", bzd_nodejs_oci_ = "bzd_nodejs_oci")
load("//nodejs:private/nodejs_package.bzl", bzd_nodejs_package_ = "bzd_nodejs_package")
load("//nodejs:private/nodejs_requirements_compile.bzl", bzd_nodejs_requirements_compile_ = "bzd_nodejs_requirements_compile")
load("//nodejs:private/nodejs_static.bzl", bzd_nodejs_static_ = "bzd_nodejs_static")
load("//nodejs:private/nodejs_web_binary.bzl", bzd_nodejs_web_binary_ = "bzd_nodejs_web_binary")
load("//nodejs:private/nodejs_web_library.bzl", bzd_nodejs_web_library_ = "bzd_nodejs_web_library")
load("//nodejs/static:defs.bzl", bzd_nodejs_static_sitemap_ = "bzd_nodejs_static_sitemap")

# Public API.

bzd_nodejs_requirements_compile = bzd_nodejs_requirements_compile_
bzd_nodejs_oci = bzd_nodejs_oci_
bzd_nodejs_install = bzd_nodejs_install_
bzd_nodejs_library = bzd_nodejs_library_
bzd_nodejs_package = bzd_nodejs_package_
bzd_nodejs_extern_binary = bzd_nodejs_extern_binary_
bzd_nodejs_web_library = bzd_nodejs_web_library_
bzd_nodejs_web_binary = bzd_nodejs_web_binary_
bzd_nodejs_static = bzd_nodejs_static_
bzd_nodejs_static_sitemap = bzd_nodejs_static_sitemap_

def bzd_nodejs_binary(name, main, data = [], tools = [], deps = [], packages = [], srcs = [], apis = [], tags = [], **kwargs):
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

def bzd_nodejs_test(name, main, data = [], tools = [], deps = [], packages = [], srcs = [], apis = [], tags = [], **kwargs):
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
        executor = Label("//toolchain/mocha"),
        install = name + ".install",
        tags = ["nodejs"] + tags,
        **kwargs
    )
