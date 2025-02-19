"""Openocd for esp32."""

load("@bzd_lib//:repository_multiplatform_maker.bzl", "repository_multiplatform_maker")

def esp32_openocd(name = "esp32_openocd"):
    """ESP32 Openocd factory function."""

    repository_multiplatform_maker(
        name = name,
        repositories = [{
            "archive": {
                "build_file": "@bzd_toolchain_cc//toolchains/esp_idf/openocd:linux_x86_64.BUILD",
                "integrity": "sha256-c2XSFTPZhgAo9Liqk/1vpd/mrp5wsrm2oB1DByoVugw=",
                "strip_prefix": "linux_x86_64_v0.12.0-esp32-20241016",
                "urls": [
                    "https://datalocal.blaizard.com/file/bzd/openocd/esp32/linux_x86_64_v0.12.0-esp32-20241016.tar.xz",
                    "https://data.blaizard.com/file/bzd/openocd/esp32/linux_x86_64_v0.12.0-esp32-20241016.tar.xz",
                ],
            },
            "platforms": [
                "linux-x86_64",
            ],
        }],
        expose = {
            "openocd": "openocd",
        },
    )

    return name, {}
