"""Esptool for esp32."""

load("@bzd_lib//:repository_multiplatform_maker.bzl", "repository_multiplatform_maker")

def esptool(_module_ctx):
    """esptool factory function.

    Args:
        _module_ctx: The module context.
    """

    repository_multiplatform_maker(
        name = "esptool",
        repositories = [{
            "archive": {
                "build_file": "@bzd_toolchain_cc//:fragments/esp_idf/esptool/esptool.BUILD",
                "sha256": "b317ce2a532c15eb55a70e290169b9bf25a514fbdb52dce43a3bd5c86fd4a49c",
                "strip_prefix": "esptool-4.3",
                "urls": [
                    "https://datalocal.blaizard.com/file/bzd/esptool/esptool-4.3.zip",
                    "https://data.blaizard.com/file/bzd/esptool/esptool-4.3.zip",
                ],
            },
            "platforms": [
                "linux-x86_64",
            ],
        }],
        expose = {
            "esptool": "esptool_binary",
        },
    )
