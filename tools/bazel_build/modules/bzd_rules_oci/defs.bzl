"""Registry for docker images used in this repository."""

load("//:private/oci_application.bzl", bzd_oci_application_ = "bzd_oci_application")
load("//:private/oci_binary.bzl", bzd_oci_binary_ = "bzd_oci_binary")
load("//:private/oci_image.bzl", bzd_oci_image_ = "bzd_oci_image")
load("//:private/oci_images_config.bzl", bzd_oci_images_config_ = "bzd_oci_images_config")
load("//:private/oci_load.bzl", bzd_oci_load_ = "bzd_oci_load")
load("//:private/oci_pull.bzl", bzd_oci_pull_ = "bzd_oci_pull")
load("//:private/oci_push.bzl", bzd_oci_push_ = "bzd_oci_push")

bzd_oci_application = bzd_oci_application_
bzd_oci_images_config = bzd_oci_images_config_
bzd_oci_pull = bzd_oci_pull_
bzd_oci_push = bzd_oci_push_
bzd_oci_binary = bzd_oci_binary_
bzd_oci_image = bzd_oci_image_
bzd_oci_load = bzd_oci_load_
