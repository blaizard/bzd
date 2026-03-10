"""Extension to download dependencies."""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bzd_lib//:repository_multiplatform_maker.bzl", "repository_multiplatform_maker")

def _load_cc_sdks(_module_ctx):
    http_archive(
        name = "esp32_xtensa_lx6_sdk",
        build_file = "//hal/cc:esp32_xtensa_lx6_sdk.BUILD",
        sha256 = "e0577241108d945f1ff1a4e0f3ef8c23685a8f2ca3cfa8328c6b69fadced4df8",
        strip_prefix = "esp32_xtensa_lx6_sdk",
        urls = [
            "https://datalocal.blaizard.com/file/bzd/sdk/esp32/cc/esp32_xtensa_lx6_sdk_v5.4.tar.xz",
            "https://data.blaizard.com/file/bzd/sdk/esp32/cc/esp32_xtensa_lx6_sdk_v5.4.tar.xz",
        ],
    )

    http_archive(
        name = "esp32s3_xtensa_lx7_sdk",
        build_file = "//hal/cc:esp32s3_xtensa_lx7_sdk.BUILD",
        sha256 = "ebcfd902bd5896b9e33c46a819ee19bdd365ab34e50fc6f9f3127efe43fc769d",
        strip_prefix = "esp32_xtensa_lx6_sdk",
        urls = [
            "https://datalocal.blaizard.com/file/bzd/sdk/esp32s3/cc/esp32s3_xtensa_lx7_sdk_v5.4.tar.xz",
            "https://data.blaizard.com/file/bzd/sdk/esp32s3/cc/esp32s3_xtensa_lx7_sdk_v5.4.tar.xz",
        ],
    )

def _load_tools(_module_ctx):
    repository_multiplatform_maker(
        name = "esptool",
        repositories = [{
            "archive": {
                "build_file": "//tools/esptool:esptool.BUILD",
                "sha256": "b317ce2a532c15eb55a70e290169b9bf25a514fbdb52dce43a3bd5c86fd4a49c",
                "strip_prefix": "esptool-4.3",
                "urls": [
                    "https://datalocal.blaizard.com/file/bzd/esptool/esptool-4.3.zip",
                    "https://data.blaizard.com/file/bzd/esptool/esptool-4.3.zip",
                ],
            },
            "platforms": [
                Label("@bzd_platforms//al_isa:linux-x86_64"),
            ],
        }],
        expose = {
            "esptool": "esptool_binary",
        },
    )

    repository_multiplatform_maker(
        name = "qemu",
        repositories = [{
            "archive": {
                "build_file": "//tools/qemu:linux_x86_64.BUILD",
                "integrity": "sha256-dc44obKaVGE46RHozHRb2wwkQQu5+ZwH+7v9q2eHo70=",
                "strip_prefix": "linux_x86_64_20240219",
                "urls": [
                    "https://datalocal.blaizard.com/file/bzd/qemu/esp32/linux_x86_64_20240219.tar.xz",
                    "https://data.blaizard.com/file/bzd/qemu/esp32/linux_x86_64_20240219.tar.xz",
                ],
            },
            "platforms": [
                Label("@bzd_platforms//al_isa:linux-x86_64"),
            ],
        }],
        expose = {
            "qemu-system-xtensa": "qemu-system-xtensa",
        },
    )

    repository_multiplatform_maker(
        name = "openocd",
        repositories = [{
            "archive": {
                "build_file": "//tools/openocd:linux_x86_64.BUILD",
                "integrity": "sha256-c2XSFTPZhgAo9Liqk/1vpd/mrp5wsrm2oB1DByoVugw=",
                "strip_prefix": "linux_x86_64_v0.12.0-esp32-20241016",
                "urls": [
                    "https://datalocal.blaizard.com/file/bzd/openocd/esp32/linux_x86_64_v0.12.0-esp32-20241016.tar.xz",
                    "https://data.blaizard.com/file/bzd/openocd/esp32/linux_x86_64_v0.12.0-esp32-20241016.tar.xz",
                ],
            },
            "platforms": [
                Label("@bzd_platforms//al_isa:linux-x86_64"),
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
