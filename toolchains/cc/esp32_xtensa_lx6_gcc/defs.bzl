load("//tools/bazel_build/toolchains/cc:defs.bzl", "toolchain_maker", "toolchain_merge")
load("//toolchains/cc/fragments/esptool:defs.bzl", "toolchain_fragment_esptool")
load("//toolchains/cc/fragments/esp32_xtensa_lx6_sdk:defs.bzl", "toolchain_fragment_esp32_xtensa_lx6_sdk")
#load("//toolchains/cc/fragments/esp32_xtensa_lx6_gcc:defs.bzl", "toolchain_fragment_esp32_xtensa_lx6_gcc")
load("//toolchains/cc/fragments/esp32_xtensa_lx6_gcc_10.2.0:defs.bzl", "toolchain_fragment_esp32_xtensa_lx6_gcc")

def load_esp32_xtensa_lx6_gcc():
    toolchain_definition = {
        "host_platforms": [
            "@//tools/bazel_build/platforms:linux_x86_64",
        ],
        "exec_compatible_with": [
            "@//tools/bazel_build/platforms/al:linux",
            "@//tools/bazel_build/platforms/isa:x86_64",
        ],
        "platforms": [
            "@//tools/bazel_build/platforms:esp32_xtensa_lx6",
        ],
        "target_compatible_with": [
            "@//tools/bazel_build/platforms/al:linux",
            "@//tools/bazel_build/platforms/isa:xtensa_lx6",
        ],
    }

    toolchain_definition = toolchain_merge(toolchain_definition, toolchain_fragment_esp32_xtensa_lx6_gcc())
    toolchain_definition = toolchain_merge(toolchain_definition, toolchain_fragment_esptool())
    toolchain_definition = toolchain_merge(toolchain_definition, toolchain_fragment_esp32_xtensa_lx6_sdk())

    toolchain_maker(
        name = "esp32_xtensa_lx6_gcc",
        implementation = "linux_gcc",
        definition = toolchain_definition,
    )
