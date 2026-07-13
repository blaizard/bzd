"""Registry for docker images used in this repository."""

load("//:private/oci_application.bzl", bzd_oci_application_ = "bzd_oci_application")
load("//:private/oci_image.bzl", bzd_oci_image_ = "bzd_oci_image")
load("//:private/oci_image_from_binaries.bzl", bzd_oci_image_from_binaries_ = "bzd_oci_image_from_binaries")
load("//:private/oci_image_from_binary.bzl", bzd_oci_image_from_binary_ = "bzd_oci_image_from_binary")
load("//:private/oci_load.bzl", bzd_oci_load_ = "bzd_oci_load")
load("//:private/oci_pull.bzl", bzd_oci_pull_ = "bzd_oci_pull")
load("//:private/oci_push.bzl", bzd_oci_push_ = "bzd_oci_push")

bzd_oci_application = bzd_oci_application_
bzd_oci_pull = bzd_oci_pull_
bzd_oci_push = bzd_oci_push_
bzd_oci_image = bzd_oci_image_
bzd_oci_image_from_binary = bzd_oci_image_from_binary_
bzd_oci_image_from_binaries = bzd_oci_image_from_binaries_
bzd_oci_load = bzd_oci_load_
