load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

filegroup(
    name = "files",
    srcs = glob([
        "bin/*",
        "ld/**/*",
        "lib/**/*",
    ]),
)

cc_library(
    name = "sdk",
    srcs = [
        "bzd/main.cpp",
    ],
    hdrs = glob([
        "include/**/*",
    ]),
    includes = [
        "include",
        "include/app_trace",
        "include/app_update",
        "include/driver",
        "include/esp32",
        "include/freertos",
        "include/heap",
        "include/soc",
    ],
    linkopts = [
        "-nostdlib",
        "-Wl,--start-group",

        # Linker scripts
        "-Tesp32_out.ld",
        "-Tesp32.project.ld",
        "-Tesp32.peripherals.ld",
        "-Tesp32.rom.ld",
        "-Tesp32.rom.libgcc.ld",
        "-Tesp32.rom.syscalls.ld",
        "-Tesp32.rom.newlib-data.ld",
        "-Tesp32.rom.newlib-funcs.ld",

        # Libraries
        "-lapp_trace",
        "-lapp_update",
        "-lasio",
        "-lbootloader_support",
        "-lbt",
        "-lcbor",
        "-lcoap",
        "-lconsole",
        "-lcxx",
        "-ldriver",
        "-lefuse",
        "-lesp-tls",
        "-lesp32",
        "-lesp_adc_cal",
        "-lesp_common",
        "-lesp_eth",
        "-lesp_event",
        "-lesp_gdbstub",
        "-lesp_http_client",
        "-lesp_http_server",
        "-lesp_https_ota",
        "-lesp_local_ctrl",
        "-lesp_netif",
        "-lesp_ringbuf",
        "-lesp_rom",
        "-lesp_serial_slave_link",
        "-lesp_websocket_client",
        "-lesp_wifi",
        "-lcore",
        "-lrtc",
        "-lnet80211",
        "-lpp",
        "-lsmartconfig",
        "-lcoexist",
        "-lespnow",
        "-lphy",
        "-lmesh",
        "-lespcoredump",
        "-lexpat",
        "-lfatfs",
        "-lfreemodbus",
        "-lfreertos",
        "-lheap",
        "-lidf_test",
        "-ljsmn",
        "-ljson",
        "-llibsodium",
        "-llog",
        "-llwip",
        "-lmbedtls",
        "-lmdns",
        "-lmqtt",
        "-lnewlib",
        "-lc",
        "-lm",
        "-lnghttp",
        "-lnvs_flash",
        "-lopenssl",
        "-lperfmon",
        "-lprotobuf-c",
        "-lprotocomm",
        "-lpthread",
        "-lsdmmc",
        "-lsoc",
        "-lspi_flash",
        "-lspiffs",
        "-ltcp_transport",
        "-ltcpip_adapter",
        "-lulp",
        "-lunity",
        "-lvfs",
        "-lwear_levelling",
        "-lwifi_provisioning",
        "-lwpa_supplicant",
        "-lxtensa",
        "-lhal",
        "-lgcc",
        "-lstdc++",
        "-lgcov",
        "-Wl,--end-group",
        "-fno-rtti",
    ],
    visibility = ["//visibility:public"],
    alwayslink = True,
)
