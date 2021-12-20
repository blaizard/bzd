load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def toolchain_fragment_esptool():
    tools_package_name = "esptool"

    http_archive(
        name = tools_package_name,
        build_file = "//toolchains/cc/fragments/esptool:{}.BUILD".format(tools_package_name),
        urls = [
            "https://github.com/espressif/esptool/archive/v2.8.zip",
        ],
        strip_prefix = "esptool-2.8",
        sha256 = "5a911d6fcf3d837c7d4052b7e0f928e9e00823a40cc8d7cb277c623e2442b31d",
    )

    return {
        "app_prepare": "@//toolchains/cc/fragments/esptool:esp32_prepare",
        "app_metadatas": [
            "@//toolchains/cc/fragments/esptool:esp32_metadata",
        ],
        "app_executors": {
            "@//toolchains/cc/fragments/esptool:executor_uart": "default,uart",
            "@//toolchains/cc/fragments/esptool:executor_qemu": "sim",
        },
    }
