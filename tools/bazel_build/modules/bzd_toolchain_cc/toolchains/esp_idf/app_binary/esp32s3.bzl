"""Application binary functor."""

def app_binary():
    """Application binary factory function."""

    return None, {
        "app_build": [
            ":app_build_to_image",
        ],
        "app_executors": {
            ":app_executor_qemu": "default,sim",
            ":app_executor_uart": "uart",
        },
        "app_metadata": [
            Label("//toolchains/esp_idf/esptool:metadata"),
        ],
        "build_files": [
            Label("//toolchains/esp_idf/app_binary:esp32s3.BUILD"),
        ],
    }
