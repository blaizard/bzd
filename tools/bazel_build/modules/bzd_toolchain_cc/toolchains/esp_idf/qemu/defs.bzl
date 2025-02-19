"""Qemu for esp32."""

load("@bzd_lib//:repository_multiplatform_maker.bzl", "repository_multiplatform_maker")

def esp32_qemu(name = "esp32_qemu"):
    """ESP32 QEMU factory function."""

    repository_multiplatform_maker(
        name = name,
        repositories = [{
            "archive": {
                "build_file": "@bzd_toolchain_cc//toolchains/esp_idf/qemu:linux_x86_64.BUILD",
                "integrity": "sha256-dc44obKaVGE46RHozHRb2wwkQQu5+ZwH+7v9q2eHo70=",
                "strip_prefix": "linux_x86_64_20240219",
                "urls": [
                    "https://datalocal.blaizard.com/file/bzd/qemu/esp32/linux_x86_64_20240219.tar.xz",
                    "https://data.blaizard.com/file/bzd/qemu/esp32/linux_x86_64_20240219.tar.xz",
                ],
            },
            "platforms": [
                "linux-x86_64",
            ],
        }],
        expose = {
            "qemu-system-xtensa": "qemu-system-xtensa",
        },
    )

    return name, {}
