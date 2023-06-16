load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def toolchain_fragment_esptool():
    http_archive(
        name = "esptool",
        build_file = "//toolchains/cc/fragments/esptool:esptool.BUILD",
        urls = [
            "http://data.blaizard.com/file/bzd/esptool/esptool-4.3.zip",
        ],
        strip_prefix = "esptool-4.3",
        sha256 = "b317ce2a532c15eb55a70e290169b9bf25a514fbdb52dce43a3bd5c86fd4a49c",
    )

    return {
        "app_build": [
            "@//toolchains/cc/fragments/esptool:esp32_build",
        ],
        "app_metadata": [
            "@//toolchains/cc/fragments/esptool:esp32_metadata",
        ],
        "app_executors": {
            "@//toolchains/cc/fragments/esptool:executor_uart": "uart",
            "@//toolchains/cc/fragments/esptool:executor_qemu": "default,sim",
        },
    }
