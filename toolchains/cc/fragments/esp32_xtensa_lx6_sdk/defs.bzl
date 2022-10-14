load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def toolchain_fragment_esp32_xtensa_lx6_sdk():
    sdk_package_name = "esp32_xtensa_lx6_sdk"

    version = "b915bb95a1ce9a84eb3dd2d9fb2f983d2d16034f"
    http_archive(
        name = sdk_package_name,
        build_file = "//toolchains/cc/fragments/esp32_xtensa_lx6_sdk:{}.BUILD".format(sdk_package_name),
        urls = [
            "https://github.com/blaizard/bzd-esp32-xtensa-sdk/archive/{}.zip".format(version),
        ],
        strip_prefix = "bzd-esp32-xtensa-sdk-{}".format(version),
        sha256 = "614770cbd0fa4c391e7dad03597b6ef3b4d3097b096704305b9f7cd91ceb6279",
    )

    return {
        "builtin_include_directories": [
            "%package(@{}//)%/include".format(sdk_package_name),
        ],
        "system_directories": [
            "external/{}/include".format(sdk_package_name),
        ],
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
            "-DIDF_VER=\"v5.0-dev-3290-g01d014c42d-dirty\"",
            "-DESP_PLATFORM",
            "-D_POSIX_READER_WRITER_LOCKS",
        ],
        "linker_dirs": [
            "external/{}/ld".format(sdk_package_name),
            "external/{}/lib".format(sdk_package_name),
        ],
        "link_flags": [
            "-mlongcalls",
            "-lxtensa",
            "-lesp_ringbuf",
            "-lefuse",
            "-ldriver",
            "-lesp_pm",
            "-lmbedtls",
            "-lapp_update",
            "-lbootloader_support",
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
            "-lvfs",
            "-lesp_eth",
            "-lesp_netif",
            "-lesp_event",
            "-lnvs_flash",
            "-lesp_phy",
            "-lwpa_supplicant",
            "-lesp_wifi",
            "-llwip",
            "-lasio",
            "-lunity",
            "-lcmock",
            "-lconsole",
            "-lhttp_parser",
            "-lesp-tls",
            "-lesp_adc_cal",
            "-lesp_gdbstub",
            "-lesp_hid",
            "-ltcp_transport",
            "-lesp_http_client",
            "-lesp_http_server",
            "-lesp_https_ota",
            "-lesp_lcd",
            "-lprotobuf-c",
            "-lprotocomm",
            "-lmdns",
            "-lesp_local_ctrl",
            "-lsdmmc",
            "-lesp_serial_slave_link",
            "-lespcoredump",
            "-lwear_levelling",
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
            "-lasio",
            "-lcmock",
            "-lunity",
            "-lesp_adc_cal",
            "-lesp_hid",
            "-lesp_lcd",
            "-lesp_local_ctrl",
            "-lmdns",
            "-lespcoredump",
            "-lfatfs",
            "-lwear_levelling",
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
            "-lapp_update",
            "-lbootloader_support",
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
            "-lvfs",
            "-lesp_eth",
            "-lesp_netif",
            "-lesp_event",
            "-lnvs_flash",
            "-lesp_phy",
            "-lwpa_supplicant",
            "-lesp_wifi",
            "-llwip",
            "-lhttp_parser",
            "-lesp-tls",
            "-lesp_gdbstub",
            "-ltcp_transport",
            "-lesp_http_client",
            "-lesp_http_server",
            "-lesp_https_ota",
            "-lsdmmc",
            "-lesp_serial_slave_link",
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
            "-lapp_update",
            "-lbootloader_support",
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
            "-lvfs",
            "-lesp_eth",
            "-lesp_netif",
            "-lesp_event",
            "-lnvs_flash",
            "-lesp_phy",
            "-lwpa_supplicant",
            "-lesp_wifi",
            "-llwip",
            "-lhttp_parser",
            "-lesp-tls",
            "-lesp_gdbstub",
            "-ltcp_transport",
            "-lesp_http_client",
            "-lesp_http_server",
            "-lesp_https_ota",
            "-lsdmmc",
            "-lesp_serial_slave_link",
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
            "-lapp_update",
            "-lbootloader_support",
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
            "-lvfs",
            "-lesp_eth",
            "-lesp_netif",
            "-lesp_event",
            "-lnvs_flash",
            "-lesp_phy",
            "-lwpa_supplicant",
            "-lesp_wifi",
            "-llwip",
            "-lhttp_parser",
            "-lesp-tls",
            "-lesp_gdbstub",
            "-ltcp_transport",
            "-lesp_http_client",
            "-lesp_http_server",
            "-lesp_https_ota",
            "-lsdmmc",
            "-lesp_serial_slave_link",
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
            "-lapp_update",
            "-lbootloader_support",
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
            "-lvfs",
            "-lesp_eth",
            "-lesp_netif",
            "-lesp_event",
            "-lnvs_flash",
            "-lesp_phy",
            "-lwpa_supplicant",
            "-lesp_wifi",
            "-llwip",
            "-lhttp_parser",
            "-lesp-tls",
            "-lesp_gdbstub",
            "-ltcp_transport",
            "-lesp_http_client",
            "-lesp_http_server",
            "-lesp_https_ota",
            "-lsdmmc",
            "-lesp_serial_slave_link",
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
            "-lapp_update",
            "-lbootloader_support",
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
            "-lvfs",
            "-lesp_eth",
            "-lesp_netif",
            "-lesp_event",
            "-lnvs_flash",
            "-lesp_phy",
            "-lwpa_supplicant",
            "-lesp_wifi",
            "-llwip",
            "-lhttp_parser",
            "-lesp-tls",
            "-lesp_gdbstub",
            "-ltcp_transport",
            "-lesp_http_client",
            "-lesp_http_server",
            "-lesp_https_ota",
            "-lsdmmc",
            "-lesp_serial_slave_link",
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
            "-T",
            "esp32.rom.newlib-time.ld",
            "-u",
            "abort",
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
            "vfs_include_syscalls_impl",
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
        ],
        "linker_files": ["@{}//:files".format(sdk_package_name)],
        "static_libraries_files": ["@{}//:static_libraries_files".format(sdk_package_name)],
        "compiler_files": ["@{}//:header_files".format(sdk_package_name)],
    }
