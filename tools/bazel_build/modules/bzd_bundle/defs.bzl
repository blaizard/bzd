"""Public rules for the bundler."""

load("//private:bundle_binary.bzl", bzd_bundle_binary_ = "bzd_bundle_binary")
load("//private:bundle_tar.bzl", bzd_bundle_tar_ = "bzd_bundle_tar")

bzd_bundle_binary = bzd_bundle_binary_
bzd_bundle_tar = bzd_bundle_tar_
