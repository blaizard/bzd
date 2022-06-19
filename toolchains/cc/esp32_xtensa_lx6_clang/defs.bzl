load("//tools/bazel_build/toolchains/cc:defs.bzl", "toolchain_maker", "toolchain_merge")
load("//toolchains/cc/fragments/esptool:defs.bzl", "toolchain_fragment_esptool")
load("//toolchains/cc/fragments/esp32_xtensa_lx6_sdk:defs.bzl", "toolchain_fragment_esp32_xtensa_lx6_sdk")
load("//toolchains/cc/fragments/esp32_xtensa_lx6_clang_14.0.0:defs.bzl", "toolchain_fragment_esp32_xtensa_lx6_clang")

def load_esp32_xtensa_lx6_clang():
    toolchain_definition = {
        "exec_compatible_with": [
            "@platforms//os:linux",
            "@platforms//cpu:x86_64",
        ],
        "target_compatible_with": [
            "@platforms//os:linux",
            "@//tools/bazel_build/platforms/isa:xtensa_lx6",
        ],
    }

    toolchain_definition = toolchain_merge(toolchain_definition, toolchain_fragment_esp32_xtensa_lx6_clang())
    toolchain_definition = toolchain_merge(toolchain_definition, toolchain_fragment_esptool())
    toolchain_definition = toolchain_merge(toolchain_definition, toolchain_fragment_esp32_xtensa_lx6_sdk())

    toolchain_maker(
        name = "esp32_xtensa_lx6_clang",
        implementation = "linux_clang",
        definition = toolchain_definition,
    )
