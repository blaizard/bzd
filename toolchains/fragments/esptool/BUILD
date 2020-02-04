filegroup(
    name = "esp32_bin",
    srcs = glob([
        "bin/esp32/*",
    ]),
)

sh_binary(
    name = "esp32_prepare",
    srcs = [
        "esp32_prepare.sh",
    ],
    data = [
        "@esptool//:tools",
    ],
    visibility = ["//visibility:public"],
)

sh_binary(
    name = "esp32_exec",
    srcs = [
        "esp32_exec.sh",
    ],
    data = [
        ":esp32_bin",
        "@esptool//:tools",
    ],
    visibility = ["//visibility:public"],
)
