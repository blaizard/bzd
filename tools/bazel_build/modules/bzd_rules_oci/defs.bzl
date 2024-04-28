"""Registry for docker images used in this repository."""

load("@bzd_rules_oci//:private/oci_image.bzl", bzd_oci_image_ = "bzd_oci_image")
load("@bzd_rules_oci//:private/oci_images_config.bzl", bzd_oci_images_config_ = "bzd_oci_images_config")
load("@bzd_rules_oci//:private/oci_pull.bzl", bzd_oci_pull_ = "bzd_oci_pull")
load("@bzd_rules_oci//:private/oci_push.bzl", bzd_oci_push_ = "bzd_oci_push")
load("@bzd_rules_oci//:private/oci_tarball.bzl", bzd_oci_tarball_ = "bzd_oci_tarball")

bzd_oci_images_config = bzd_oci_images_config_
bzd_oci_pull = bzd_oci_pull_
bzd_oci_push = bzd_oci_push_
bzd_oci_image = bzd_oci_image_
bzd_oci_tarball = bzd_oci_tarball_
