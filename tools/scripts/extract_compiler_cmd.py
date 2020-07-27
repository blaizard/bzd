#!/usr/bin/python

import shlex
import os
import re
from typing import Dict, Mapping, Callable, Any

COMMAND_LINE = 'xtensa-esp32-elf-gcc -std=gnu99 -Og -ggdb -Wno-frame-address -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -mlongcalls -nostdlib -Wall -Werror=all -Wno-error=unused-function -Wno-error=unused-but-set-variable -Wno-error=unused-variable -Wno-error=deprecated-declarations -Wextra -Wno-unused-parameter -Wno-sign-compare -Wno-old-style-declaration -DESP_PLATFORM -D IDF_VER=\"v4.1-dev-1935-g647cb628a-dirty\" -MMD -MP   -D_GNU_SOURCE -DMBEDTLS_CONFIG_FILE=' "mbedtls/esp_config.h" ' -DHAVE_CONFIG_H -DUNITY_INCLUDE_CONFIG_H -I /home/blaise/projects/esp-idf/examples/peripherals/i2c/i2c_self_test/main/include -I /home/blaise/projects/esp-idf/components/app_trace/include -I /home/blaise/projects/esp-idf/components/app_update/include -I /home/blaise/projects/esp-idf/components/asio/asio/asio/include -I /home/blaise/projects/esp-idf/components/asio/port/include -I /home/blaise/projects/esp-idf/components/bootloader_support/include -I /home/blaise/projects/esp-idf/components/bt/include -I /home/blaise/projects/esp-idf/components/cbor/port/include -I /home/blaise/projects/esp-idf/components/coap/port/include -I /home/blaise/projects/esp-idf/components/coap/port/include/coap -I /home/blaise/projects/esp-idf/components/coap/libcoap/include -I /home/blaise/projects/esp-idf/components/coap/libcoap/include/coap2 -I /home/blaise/projects/esp-idf/components/console -I /home/blaise/projects/esp-idf/components/driver/include -I /home/blaise/projects/esp-idf/components/driver/esp32/include -I /home/blaise/projects/esp-idf/components/efuse/esp32/include -I /home/blaise/projects/esp-idf/components/efuse/include -I /home/blaise/projects/esp-idf/components/esp-tls -I /home/blaise/projects/esp-idf/components/esp-tls/private_include -I /home/blaise/projects/esp-idf/components/esp32/include -I /home/blaise/projects/esp-idf/components/esp_adc_cal/include -I /home/blaise/projects/esp-idf/components/esp_common/include -I /home/blaise/projects/esp-idf/components/esp_eth/include -I /home/blaise/projects/esp-idf/components/esp_event/include -I /home/blaise/projects/esp-idf/components/esp_gdbstub/include -I /home/blaise/projects/esp-idf/components/esp_http_client/include -I /home/blaise/projects/esp-idf/components/esp_http_server/include -I /home/blaise/projects/esp-idf/components/esp_https_ota/include -I /home/blaise/projects/esp-idf/components/esp_local_ctrl/include -I /home/blaise/projects/esp-idf/components/esp_netif/include -I /home/blaise/projects/esp-idf/components/esp_ringbuf/include -I /home/blaise/projects/esp-idf/components/esp_rom/include -I /home/blaise/projects/esp-idf/components/esp_serial_slave_link/include -I /home/blaise/projects/esp-idf/components/esp_websocket_client/include -I /home/blaise/projects/esp-idf/components/esp_wifi/include -I /home/blaise/projects/esp-idf/components/esp_wifi/esp32/include -I /home/blaise/projects/esp-idf/components/espcoredump/include -I /home/blaise/projects/esp-idf/components/expat/expat/expat/lib -I /home/blaise/projects/esp-idf/components/expat/port/include -I /home/blaise/projects/esp-idf/components/fatfs/diskio -I /home/blaise/projects/esp-idf/components/fatfs/vfs -I /home/blaise/projects/esp-idf/components/fatfs/src -I /home/blaise/projects/esp-idf/components/freemodbus/common/include -I /home/blaise/projects/esp-idf/components/freertos/include -I /home/blaise/projects/esp-idf/components/heap/include -I /home/blaise/projects/esp-idf/components/idf_test/include -I /home/blaise/projects/esp-idf/components/jsmn/include -I /home/blaise/projects/esp-idf/components/json/cJSON -I /home/blaise/projects/esp-idf/components/libsodium/libsodium/src/libsodium/include -I /home/blaise/projects/esp-idf/components/libsodium/port_include -I /home/blaise/projects/esp-idf/components/log/include -I /home/blaise/projects/esp-idf/components/lwip/include/apps -I /home/blaise/projects/esp-idf/components/lwip/include/apps/sntp -I /home/blaise/projects/esp-idf/components/lwip/lwip/src/include -I /home/blaise/projects/esp-idf/components/lwip/port/esp32/include -I /home/blaise/projects/esp-idf/components/lwip/port/esp32/include/arch -I /home/blaise/projects/esp-idf/components/mbedtls/port/include -I /home/blaise/projects/esp-idf/components/mbedtls/mbedtls/include -I /home/blaise/projects/esp-idf/components/mdns/include -I /home/blaise/projects/esp-idf/components/mqtt/esp-mqtt/include -I /home/blaise/projects/esp-idf/components/newlib/platform_include -I /home/blaise/projects/esp-idf/components/nghttp/port/include -I /home/blaise/projects/esp-idf/components/nghttp/nghttp2/lib/includes -I /home/blaise/projects/esp-idf/components/nvs_flash/include -I /home/blaise/projects/esp-idf/components/openssl/include -I /home/blaise/projects/esp-idf/components/perfmon/include -I /home/blaise/projects/esp-idf/components/protobuf-c/protobuf-c -I /home/blaise/projects/esp-idf/components/protocomm/include/common -I /home/blaise/projects/esp-idf/components/protocomm/include/security -I /home/blaise/projects/esp-idf/components/protocomm/include/transports -I /home/blaise/projects/esp-idf/components/pthread/include -I /home/blaise/projects/esp-idf/components/sdmmc/include -I /home/blaise/projects/esp-idf/components/soc/esp32/include -I /home/blaise/projects/esp-idf/components/soc/include -I /home/blaise/projects/esp-idf/components/spi_flash/include -I /home/blaise/projects/esp-idf/components/spiffs/include -I /home/blaise/projects/esp-idf/components/tcp_transport/include -I /home/blaise/projects/esp-idf/components/tcpip_adapter/include -I /home/blaise/projects/esp-idf/components/ulp/include -I /home/blaise/projects/esp-idf/components/unity/include -I /home/blaise/projects/esp-idf/components/unity/unity/src -I /home/blaise/projects/esp-idf/components/vfs/include -I /home/blaise/projects/esp-idf/components/wear_levelling/include -I /home/blaise/projects/esp-idf/components/wifi_provisioning/include -I /home/blaise/projects/esp-idf/components/wpa_supplicant/include -I /home/blaise/projects/esp-idf/components/wpa_supplicant/port/include -I /home/blaise/projects/esp-idf/components/wpa_supplicant/include/esp_supplicant -I /home/blaise/projects/esp-idf/components/xtensa/include -I /home/blaise/projects/esp-idf/components/xtensa/esp32/include -I /home/blaise/projects/esp-idf/examples/peripherals/i2c/i2c_self_test/build/include  -I . -c /home/blaise/projects/esp-idf/examples/peripherals/i2c/i2c_self_test/main/i2c_example_main.c -o i2c_example_main.o'
"""
Parse a command line and extract important information.
"""

parseState = None


def parse(cmdStr: str) -> Mapping[str, Any]:

	results: Dict[str, Any] = {"files": {"lds": [], "libs": [], "includes": []}, "linker": {"lds": [], "libs": []}}

	libSearchPathList = ["/"]
	global parseState
	parseState = None

	def _handleArg(key: str, arg: str, callback: Callable[[str], None]) -> None:
		global parseState
		if parseState == key:
			parseState = None
			callback(arg.strip())
		elif arg.startswith(key):
			if arg == key:
				parseState = key
			else:
				callback(arg[len(key):].strip())

	def addLib(name: str) -> None:
		fileName = name if re.match(r'lib.*\.a', os.path.basename(name)) else "lib{}.a".format(name)
		for path in libSearchPathList:
			p = os.path.join(path, fileName)
			if os.path.isfile(p):
				results["files"]["libs"].append(p)
				m = re.match(r'lib(.*)\.a', os.path.basename(p))
				assert m
				results["linker"]["libs"].append(m.group(1))
				return
		print("WARNING: cannot locate library {}".format(name))

	def addLd(fileName: str) -> None:
		for path in libSearchPathList:
			p = os.path.join(path, fileName)
			if os.path.isfile(p):
				results["files"]["lds"].append(p)
				results["linker"]["lds"].append(os.path.basename(fileName))
				return
		print("WARNING: cannot locate linker script {}".format(fileName))

	for arg in shlex.split(cmdStr):
		# Lib search path
		_handleArg("-L", arg, lambda x: libSearchPathList.append(x))
		# Lib
		_handleArg("-l", arg, lambda x: addLib(x))
		if arg.endswith(".a"):
			addLib(arg)
		# Linker script
		_handleArg("-T", arg, lambda x: addLd(x))
		# Include path
		_handleArg("-I", arg, lambda x: results["files"]["includes"].append(x))  # type: ignore

	return results


"""
This utility extracts compilation information from the command line.
Build your application with make VERBOSE=1 and copy/paste the targted command line into COMMAND_LINE.
"""
if __name__ == "__main__":

	result = parse(COMMAND_LINE)

	print("# Linker scripts")
	for p in result["lds"]:
		print("cp \"{}\" .".format(p))
	print("# Libraries")
	for p in result["libs"]:
		print("cp \"{}\" .".format(p))
	print("# Linker arguments")
	for p in result["linker"]:
		print(p)
	print("# Include")
	for p in result["includes"]:
		print("rsync -avm --include='*.h' -f 'hide,! */' \"{}/\" .".format(p))
