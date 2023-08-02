"""Metadata for esptool toolchains."""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def esptool(_module_ctx):
    http_archive(
        name = "esptool",
        build_file = "@bzd_toolchain_cc//fragments/esp32/esptool:esptool.BUILD",
        urls = [
            "http://data.blaizard.com/file/bzd/esptool/esptool-4.3.zip",
        ],
        strip_prefix = "esptool-4.3",
        sha256 = "b317ce2a532c15eb55a70e290169b9bf25a514fbdb52dce43a3bd5c86fd4a49c",
    )

    return {
        "app_build": [
            "@esptool//:esp32_build",
        ],
        "app_executors": {
            "@esptool//:executor_qemu": "default,sim",
            "@esptool//:executor_uart_wrapped": "uart",
        },
        "app_metadata": [
            "@esptool//:esp32_metadata",
        ],
    }
