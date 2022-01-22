load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def toolchain_fragment_esptool():
    tools_package_name = "esptool"

    http_archive(
        name = tools_package_name,
        build_file = "//toolchains/cc/fragments/esptool:{}.BUILD".format(tools_package_name),
        urls = [
            "https://github.com/espressif/esptool/archive/refs/tags/v3.2.zip",
        ],
        strip_prefix = "esptool-3.2",
        sha256 = "8e3c84b071a205da7232cd320d4ebca5218c1e3d419f510e18db92e3e9928bf5",
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
