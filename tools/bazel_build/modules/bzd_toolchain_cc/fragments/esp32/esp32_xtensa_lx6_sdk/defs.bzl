"""Metadata for ESP32 toolchains."""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("//cc:toolchain.bzl", "get_location")

def esp32_xtensa_lx6_sdk(module_ctx, name = "esp32_xtensa_lx6_sdk"):
    version = "5dabe9f8a000131b0d72294f415332d8ef640126"
    http_archive(
        name = name,
        build_file = Label("//:fragments/esp32/esp32_xtensa_lx6_sdk/esp32_xtensa_lx6_sdk.BUILD"),
        urls = [
            "https://github.com/blaizard/bzd-esp32-xtensa-sdk/archive/{}.zip".format(version),
        ],
        strip_prefix = "bzd-esp32-xtensa-sdk-{}".format(version),
        sha256 = "2ca96a491b2c6c136132fe39b713afccc2a17dc163e62c26991105854a690e5e",
    )

    repository_path = get_location(module_ctx, name)
    return {
        "compile_flags": [
            "-DMBEDTLS_CONFIG_FILE=\"mbedtls/esp_config.h\"",
            "-DUNITY_INCLUDE_CONFIG_H",
            "-mlongcalls",
            "-ffunction-sections",
            "-fdata-sections",
            "-freorder-blocks",
            "-fstrict-volatile-bitfields",
            "-fno-jump-tables",
            "-fno-tree-switch-conversion",
            "-DconfigENABLE_FREERTOS_DEBUG_OCDAWARE=1",
            #"-fno-exceptions",
            "-fno-rtti",
            "-D_GNU_SOURCE",
            "-DIDF_VER=\"v5.0.1\"",
            "-DESP_PLATFORM",
            "-D_POSIX_READER_WRITER_LOCKS",
        ],
        "link_flags": [
            "-mlongcalls",
            "-lxtensa",
            "-lesp_ringbuf",
            "-lefuse",
            "-ldriver",
            "-lesp_pm",
            "-lmbedtls",
            "-lesp_app_format",
            "-lbootloader_support",
            "-lesp_partition",
            "-lapp_update",
            "-lspi_flash",
            "-lpthread",
            "-lesp_system",
            "-lesp_rom",
            "-lhal",
            "-llog",
            "-lheap",
            "-lsoc",
            "-lesp_hw_support",
            "-lfreertos",
            "-lnewlib",
            "-lcxx",
            "-lesp_common",
            "-lesp_timer",
            "-lapp_trace",
            "-lesp_event",
            "-lnvs_flash",
            "-lesp_phy",
            "-lvfs",
            "-llwip",
            "-lesp_netif",
            "-lwpa_supplicant",
            "-lesp_wifi",
            "-lunity",
            "-lcmock",
            "-lconsole",
            "-lhttp_parser",
            "-lesp-tls",
            "-lesp_adc",
            "-lesp_eth",
            "-lesp_gdbstub",
            "-lesp_hid",
            "-ltcp_transport",
            "-lesp_http_client",
            "-lesp_http_server",
            "-lesp_https_ota",
            "-lesp_lcd",
            "-lprotobuf-c",
            "-lprotocomm",
            "-lesp_local_ctrl",
            "-lespcoredump",
            "-lwear_levelling",
            "-lsdmmc",
            "-lfatfs",
            "-ljson",
            "-lmqtt",
            "-lperfmon",
            "-lspiffs",
            "-lulp",
            "-lwifi_provisioning",
            "-lmain",
            "-fno-rtti",
            "-fno-lto",
            "-lapp_trace",
            "-lapp_trace",
            "-lgcov",
            "-lapp_trace",
            "-lgcov",
            "-lcmock",
            "-lunity",
            "-lesp_hid",
            "-lesp_lcd",
            "-lesp_local_ctrl",
            "-lespcoredump",
            "-lfatfs",
            "-lwear_levelling",
            "-lsdmmc",
            "-lmqtt",
            "-lperfmon",
            "-lspiffs",
            "-lwifi_provisioning",
            "-lprotocomm",
            "-lconsole",
            "-lprotobuf-c",
            "-ljson",
            "-lxtensa",
            "-lesp_ringbuf",
            "-lefuse",
            "-ldriver",
            "-lesp_pm",
            "-lmbedtls",
            "-lesp_app_format",
            "-lbootloader_support",
            "-lesp_partition",
            "-lapp_update",
            "-lspi_flash",
            "-lpthread",
            "-lesp_system",
            "-lesp_rom",
            "-lhal",
            "-llog",
            "-lheap",
            "-lsoc",
            "-lesp_hw_support",
            "-lfreertos",
            "-lnewlib",
            "-lcxx",
            "-lesp_common",
            "-lesp_timer",
            "-lesp_event",
            "-lnvs_flash",
            "-lesp_phy",
            "-lvfs",
            "-llwip",
            "-lesp_netif",
            "-lwpa_supplicant",
            "-lesp_wifi",
            "-lhttp_parser",
            "-lesp-tls",
            "-lesp_adc",
            "-lesp_eth",
            "-lesp_gdbstub",
            "-ltcp_transport",
            "-lesp_http_client",
            "-lesp_http_server",
            "-lesp_https_ota",
            "-lulp",
            "-lmbedtls",
            "-lmbedcrypto",
            "-lmbedx509",
            "-lcoexist",
            "-lcore",
            "-lespnow",
            "-lmesh",
            "-lnet80211",
            "-lpp",
            "-lsmartconfig",
            "-lwapi",
            "-lxtensa",
            "-lesp_ringbuf",
            "-lefuse",
            "-ldriver",
            "-lesp_pm",
            "-lmbedtls",
            "-lesp_app_format",
            "-lbootloader_support",
            "-lesp_partition",
            "-lapp_update",
            "-lspi_flash",
            "-lpthread",
            "-lesp_system",
            "-lesp_rom",
            "-lhal",
            "-llog",
            "-lheap",
            "-lsoc",
            "-lesp_hw_support",
            "-lfreertos",
            "-lnewlib",
            "-lcxx",
            "-lesp_common",
            "-lesp_timer",
            "-lesp_event",
            "-lnvs_flash",
            "-lesp_phy",
            "-lvfs",
            "-llwip",
            "-lesp_netif",
            "-lwpa_supplicant",
            "-lesp_wifi",
            "-lhttp_parser",
            "-lesp-tls",
            "-lesp_adc",
            "-lesp_eth",
            "-lesp_gdbstub",
            "-ltcp_transport",
            "-lesp_http_client",
            "-lesp_http_server",
            "-lesp_https_ota",
            "-lulp",
            "-lmbedtls",
            "-lmbedcrypto",
            "-lmbedx509",
            "-lcoexist",
            "-lcore",
            "-lespnow",
            "-lmesh",
            "-lnet80211",
            "-lpp",
            "-lsmartconfig",
            "-lwapi",
            "-lxtensa",
            "-lesp_ringbuf",
            "-lefuse",
            "-ldriver",
            "-lesp_pm",
            "-lmbedtls",
            "-lesp_app_format",
            "-lbootloader_support",
            "-lesp_partition",
            "-lapp_update",
            "-lspi_flash",
            "-lpthread",
            "-lesp_system",
            "-lesp_rom",
            "-lhal",
            "-llog",
            "-lheap",
            "-lsoc",
            "-lesp_hw_support",
            "-lfreertos",
            "-lnewlib",
            "-lcxx",
            "-lesp_common",
            "-lesp_timer",
            "-lesp_event",
            "-lnvs_flash",
            "-lesp_phy",
            "-lvfs",
            "-llwip",
            "-lesp_netif",
            "-lwpa_supplicant",
            "-lesp_wifi",
            "-lhttp_parser",
            "-lesp-tls",
            "-lesp_adc",
            "-lesp_eth",
            "-lesp_gdbstub",
            "-ltcp_transport",
            "-lesp_http_client",
            "-lesp_http_server",
            "-lesp_https_ota",
            "-lulp",
            "-lmbedtls",
            "-lmbedcrypto",
            "-lmbedx509",
            "-lcoexist",
            "-lcore",
            "-lespnow",
            "-lmesh",
            "-lnet80211",
            "-lpp",
            "-lsmartconfig",
            "-lwapi",
            "-lxtensa",
            "-lesp_ringbuf",
            "-lefuse",
            "-ldriver",
            "-lesp_pm",
            "-lmbedtls",
            "-lesp_app_format",
            "-lbootloader_support",
            "-lesp_partition",
            "-lapp_update",
            "-lspi_flash",
            "-lpthread",
            "-lesp_system",
            "-lesp_rom",
            "-lhal",
            "-llog",
            "-lheap",
            "-lsoc",
            "-lesp_hw_support",
            "-lfreertos",
            "-lnewlib",
            "-lcxx",
            "-lesp_common",
            "-lesp_timer",
            "-lesp_event",
            "-lnvs_flash",
            "-lesp_phy",
            "-lvfs",
            "-llwip",
            "-lesp_netif",
            "-lwpa_supplicant",
            "-lesp_wifi",
            "-lhttp_parser",
            "-lesp-tls",
            "-lesp_adc",
            "-lesp_eth",
            "-lesp_gdbstub",
            "-ltcp_transport",
            "-lesp_http_client",
            "-lesp_http_server",
            "-lesp_https_ota",
            "-lulp",
            "-lmbedtls",
            "-lmbedcrypto",
            "-lmbedx509",
            "-lcoexist",
            "-lcore",
            "-lespnow",
            "-lmesh",
            "-lnet80211",
            "-lpp",
            "-lsmartconfig",
            "-lwapi",
            "-lxtensa",
            "-lesp_ringbuf",
            "-lefuse",
            "-ldriver",
            "-lesp_pm",
            "-lmbedtls",
            "-lesp_app_format",
            "-lbootloader_support",
            "-lesp_partition",
            "-lapp_update",
            "-lspi_flash",
            "-lpthread",
            "-lesp_system",
            "-lesp_rom",
            "-lhal",
            "-llog",
            "-lheap",
            "-lsoc",
            "-lesp_hw_support",
            "-lfreertos",
            "-lnewlib",
            "-lcxx",
            "-lesp_common",
            "-lesp_timer",
            "-lesp_event",
            "-lnvs_flash",
            "-lesp_phy",
            "-lvfs",
            "-llwip",
            "-lesp_netif",
            "-lwpa_supplicant",
            "-lesp_wifi",
            "-lhttp_parser",
            "-lesp-tls",
            "-lesp_adc",
            "-lesp_eth",
            "-lesp_gdbstub",
            "-ltcp_transport",
            "-lesp_http_client",
            "-lesp_http_server",
            "-lesp_https_ota",
            "-lulp",
            "-lmbedtls",
            "-lmbedcrypto",
            "-lmbedx509",
            "-lcoexist",
            "-lcore",
            "-lespnow",
            "-lmesh",
            "-lnet80211",
            "-lpp",
            "-lsmartconfig",
            "-lwapi",
            "-lxt_hal",
            "-u",
            "esp_app_desc",
            "-u",
            "pthread_include_pthread_impl",
            "-u",
            "pthread_include_pthread_cond_impl",
            "-u",
            "pthread_include_pthread_local_storage_impl",
            "-u",
            "pthread_include_pthread_rwlock_impl",
            "-u",
            "ld_include_highint_hdl",
            "-u",
            "start_app",
            "-u",
            "start_app_other_cores",
            "-T",
            "memory.ld",
            "-T",
            "sections.ld",
            "-u",
            "__ubsan_include",
            "-T",
            "esp32.rom.ld",
            "-T",
            "esp32.rom.api.ld",
            "-T",
            "esp32.rom.libgcc.ld",
            "-T",
            "esp32.rom.newlib-data.ld",
            "-T",
            "esp32.rom.syscalls.ld",
            "-T",
            "esp32.rom.newlib-funcs.ld",
            "-u",
            "abort",
            "-u",
            "esp_dport_access_reg_read",
            "-T",
            "esp32.peripherals.ld",
            "-u",
            "app_main",
            "-lc",
            "-lm",
            "-lnewlib",
            "-u",
            "newlib_include_heap_impl",
            "-u",
            "newlib_include_syscalls_impl",
            "-u",
            "newlib_include_pthread_impl",
            "-u",
            "newlib_include_assert_impl",
            "-u",
            "__cxa_guard_dummy",
            "-lstdc++",
            "-lpthread",
            "-lgcc",
            "-lcxx",
            "-u",
            "__cxx_fatal_exception",
            "-u",
            "include_esp_phy_override",
            "-lphy",
            "-lrtc",
            "-lesp_phy",
            "-lphy",
            "-lrtc",
            "-lesp_phy",
            "-lphy",
            "-lrtc",
            "-u",
            "vfs_include_syscalls_impl",
        ],
        "linker_dirs": [
            "{}/ld".format(repository_path),
            "{}/lib".format(repository_path),
        ],
        "system_directories": [
            "{}/include".format(repository_path),
        ],
    }
