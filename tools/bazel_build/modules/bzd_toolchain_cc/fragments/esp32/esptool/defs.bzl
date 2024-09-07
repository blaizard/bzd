"""Esptool for esp32."""

load("@bzd_lib//:multiplatform_repository.bzl", "multiplatform_repository")

def esptool(_module_ctx):
    """esptool factory function.

    Args:
        _module_ctx: The module context.
    """

    multiplatform_repository(
        name = "esptool",
        repositories = [{
            "build_file": "@bzd_toolchain_cc//:fragments/esp32/esptool/esptool.BUILD",
            "platforms": [
                "linux-x86_64",
            ],
            "sha256": "b317ce2a532c15eb55a70e290169b9bf25a514fbdb52dce43a3bd5c86fd4a49c",
            "strip_prefix": "esptool-4.3",
            "urls": [
                "https://datalocal.blaizard.com/file/bzd/esptool/esptool-4.3.zip",
                "https://data.blaizard.com/file/bzd/esptool/esptool-4.3.zip",
            ],
        }],
        expose = {
            "esptool": "esptool_binary",
        },
    )
