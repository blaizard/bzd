"""Rules for NodeJs."""

load("//nodejs:private/nodejs_executable.bzl", bzd_nodejs_binary_ = "bzd_nodejs_binary", bzd_nodejs_test_ = "bzd_nodejs_test")
load("//nodejs:private/nodejs_extern_binary.bzl", bzd_nodejs_extern_binary_ = "bzd_nodejs_extern_binary")
load("//nodejs:private/nodejs_library.bzl", bzd_nodejs_library_ = "bzd_nodejs_library")
load("//nodejs:private/nodejs_oci_binary.bzl", bzd_nodejs_oci_binary_ = "bzd_nodejs_oci_binary")
load("//nodejs:private/nodejs_package.bzl", bzd_nodejs_package_ = "bzd_nodejs_package")
load("//nodejs:private/nodejs_requirements_compile.bzl", bzd_nodejs_requirements_compile_ = "bzd_nodejs_requirements_compile")
load("//nodejs:private/nodejs_static.bzl", bzd_nodejs_static_ = "bzd_nodejs_static")
load("//nodejs:private/nodejs_web_binary.bzl", bzd_nodejs_web_binary_ = "bzd_nodejs_web_binary")
load("//nodejs:private/nodejs_web_library.bzl", bzd_nodejs_web_library_ = "bzd_nodejs_web_library")
load("//nodejs/static:defs.bzl", bzd_nodejs_static_sitemap_ = "bzd_nodejs_static_sitemap")

# Public API.

bzd_nodejs_requirements_compile = bzd_nodejs_requirements_compile_
bzd_nodejs_library = bzd_nodejs_library_
bzd_nodejs_oci_binary = bzd_nodejs_oci_binary_
bzd_nodejs_package = bzd_nodejs_package_
bzd_nodejs_extern_binary = bzd_nodejs_extern_binary_
bzd_nodejs_web_library = bzd_nodejs_web_library_
bzd_nodejs_web_binary = bzd_nodejs_web_binary_
bzd_nodejs_static = bzd_nodejs_static_
bzd_nodejs_static_sitemap = bzd_nodejs_static_sitemap_
bzd_nodejs_binary = bzd_nodejs_binary_
bzd_nodejs_test = bzd_nodejs_test_
