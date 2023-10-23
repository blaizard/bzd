"""Application binary functor."""

load("@bzd_toolchain_cc//fragments/esp32/esptool:defs.bzl", "esptool")
load("@bzd_toolchain_cc//fragments/esp32/qemu:defs.bzl", "esp32_qemu")

def app_binary(module_ctx):
    """Application binary factory function.

    Args:
        module_ctx: The module context.
    """

    esp32_qemu(module_ctx)
    esptool(module_ctx)

    return {
        "app_build": [
            ":app_build_to_image",
        ],
        "app_executors": {
            ":app_executor_qemu": "default,sim",
            ":app_executor_uart": "uart",
        },
        "app_metadata": [
            "@bzd_toolchain_cc//fragments/esp32/esptool:esp32_metadata",
        ],
        "build_files": [
            "@bzd_toolchain_cc//fragments/esp32/app_binary/esp32_xtensa_lx6:esp32_xtensa_lx6.BUILD",
        ],
    }
