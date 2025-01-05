"""Application binary functor."""

load("//fragments/esp_idf/esptool:defs.bzl", "esptool")
load("//fragments/esp_idf/qemu:defs.bzl", "esp32_qemu")

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
            Label("//fragments/esp_idf/esptool:metadata"),
        ],
        "build_files": [
            Label("//fragments/esp_idf/app_binary:esp32.BUILD"),
        ],
    }
