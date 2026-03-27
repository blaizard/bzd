"""Extension to download dependencies."""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bzd_lib//:defs.bzl", "bzd_http_archive", "bzd_repository_multiplatform_maker")

def _load_cc_sdks(_module_ctx):
    http_archive(
        name = "esp32_xtensa_lx6_sdk",
        build_file = "//hal/cc:esp32_xtensa_lx6_sdk.BUILD.bazel",
        sha256 = "e0577241108d945f1ff1a4e0f3ef8c23685a8f2ca3cfa8328c6b69fadced4df8",
        strip_prefix = "esp32_xtensa_lx6_sdk",
        urls = [
            "https://data.blaizard.com/file/bzd/sdk/esp32/cc/esp32_xtensa_lx6_sdk_v5.4.tar.xz",
        ],
    )

    http_archive(
        name = "esp32s3_xtensa_lx7_sdk",
        build_file = "//hal/cc:esp32s3_xtensa_lx7_sdk.BUILD.bazel",
        sha256 = "ebcfd902bd5896b9e33c46a819ee19bdd365ab34e50fc6f9f3127efe43fc769d",
        strip_prefix = "esp32_xtensa_lx6_sdk",
        urls = [
            "https://data.blaizard.com/file/bzd/sdk/esp32s3/cc/esp32s3_xtensa_lx7_sdk_v5.4.tar.xz",
        ],
    )

def _load_tools(_module_ctx):
    bzd_repository_multiplatform_maker(
        name = "esptool",
        repositories = [{
            "build_file": "//tools/esptool:esptool.BUILD.bazel",
            "compatible_with": [
                Label("@bzd_platforms//al_isa:linux-x86_64"),
            ],
            "http": [
                bzd_http_archive(
                    sha256 = "b317ce2a532c15eb55a70e290169b9bf25a514fbdb52dce43a3bd5c86fd4a49c",
                    strip_prefix = "esptool-4.3",
                    url = "https://data.blaizard.com/file/bzd/esptool/esptool-4.3.zip",
                ),
            ],
        }],
        expose = {
            "esptool": "esptool_binary",
        },
    )

    bzd_repository_multiplatform_maker(
        name = "qemu",
        repositories = [{
            "build_file": "//tools/qemu:linux_x86_64.BUILD.bazel",
            "compatible_with": [
                Label("@bzd_platforms//al_isa:linux-x86_64"),
            ],
            "http": [
                bzd_http_archive(
                    integrity = "sha256-bfL5Vfftu7vjx4vdY5f83/gfKcUHMRiuqNP3XUN+9/Y=",
                    strip_prefix = "linux_x86_64_esp-develop-9.2.2-20250817",
                    url = "https://data.blaizard.com/file/bzd/qemu/esp32/linux_x86_64_esp-develop-9.2.2-20250817.tar.xz",
                ),
            ],
        }],
        expose = {
            "qemu-system-xtensa": "qemu-system-xtensa",
        },
    )

    bzd_repository_multiplatform_maker(
        name = "openocd",
        repositories = [{
            "build_file": "//tools/openocd:linux_x86_64.BUILD.bazel",
            "compatible_with": [
                Label("@bzd_platforms//al_isa:linux-x86_64"),
            ],
            "http": [
                bzd_http_archive(
                    integrity = "sha256-c2XSFTPZhgAo9Liqk/1vpd/mrp5wsrm2oB1DByoVugw=",
                    strip_prefix = "linux_x86_64_v0.12.0-esp32-20241016",
                    url = "https://data.blaizard.com/file/bzd/openocd/esp32/linux_x86_64_v0.12.0-esp32-20241016.tar.xz",
                ),
            ],
        }],
        expose = {
            "openocd": "openocd",
        },
    )

def _load_dependencies_impl(module_ctx):
    _load_cc_sdks(module_ctx)
    _load_tools(module_ctx)

load_dependencies = module_extension(
    implementation = _load_dependencies_impl,
)
