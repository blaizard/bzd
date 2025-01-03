"""Qemu for esp32."""

load("@bzd_lib//:repository_multiplatform_maker.bzl", "repository_multiplatform_maker")

def esp32_qemu(_module_ctx):
    """ESP32 QEMU factory function.

    Args:
        _module_ctx: The module context.
    """

    repository_multiplatform_maker(
        name = "esp32_qemu",
        repositories = [{
            "archive": {
                "build_file": "@bzd_toolchain_cc//:fragments/esp32/qemu/linux_x86_64_8.1.2.BUILD",
                "sha256": "df996467865470dbf7c6a80b1d921ffe23eba7be477c165c41192b4343c42d21",
                "strip_prefix": "linux_x86_64_8.1.2",
                "urls": [
                    "https://datalocal.blaizard.com/file/bzd/qemu/esp32/linux_x86_64_8.1.2.tar.xz",
                    "https://data.blaizard.com/file/bzd/qemu/esp32/linux_x86_64_8.1.2.tar.xz",
                ],
            },
            "platforms": [
                "linux-x86_64",
            ],
        }],
        expose = {
            "qemu": "qemu",
        },
    )
